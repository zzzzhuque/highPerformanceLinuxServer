//
// Created by zhuque on 2021/3/21.
//
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cassert>
#include <cstdio>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <cstdlib>
#include <sys/epoll.h>
#include <pthread.h>
#include <sys/syscall.h>

#define MAX_EVENT_NUMBER 1024
#define BUFFER_SIZE 1024
#define gettid() syscall(SYS_gettid)

struct fds {
    /*
     * epollfd: 例程描述符
     * sockfd:  监听套接字描述符
     */
    int epollfd;
    int sockfd;

    fds(int ep, int so) : epollfd(ep), sockfd(so) {}
};

int setnonblocking(int fd) {
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

/*
 * 将fd上的EPOLLIN和EPOLLET事件注册到epollfd指示的epoll内核事件表中，
 * 参数oneshot指定是否注册fd上的EPOLLONESHOT事件
 */
void addfd(int epollfd, int fd, bool oneshot) {
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;       ////< @note 设置为边缘触发
    if (oneshot) {
        event.events |= EPOLLONESHOT;       ////< @note 设置oneshot
    }
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);                     ////< @note 设置套接字非阻塞
}

/*
 * 重置fd上的事件。这样操作之后，尽管fd上的EPOLLONESHOT事件被注册，但是操作系统
 * 仍然会触发fd上的EPOLLIN事件，且只触发一次
 */
void reset_oneshot(int epollfd, int fd) {
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
}

/*
 * 工作线程
 */
void* worker(void *arg) {
    int sockfd = ((fds*)arg)->sockfd;
    int epollfd = ((fds*)arg)->epollfd;
    printf("start new thread to receive data on fd: %d\n", sockfd);
    char buf[BUFFER_SIZE];
    memset(buf, '\0', BUFFER_SIZE);
    while (true) {              ////< @note 循环读取sockfd上的数据，直到遇到EAGAIN错误
        int ret = recv(sockfd, buf, BUFFER_SIZE-1, 0);
        if (ret == 0) {
            close(sockfd);
            printf("foreiner closed the connection\n");
            break;
        } else if (ret < 0) {
            if (errno == EAGAIN) {
                reset_oneshot(epollfd, sockfd);
                printf("read later\n");
                break;
            }
        } else {
            printf("get content by thread %ld: %s\n", gettid(), buf);
            sleep(5);           ////< @note 休眠5s，模拟数据处理过程
        }
    }
    printf("end thread receiving data on fd: %d\n", sockfd);
}

int main(int argc, char *argv[]) {
    if (argc < 1) {
        printf("Usage: %s port number\n", argv[0]);
        return 1;
    }

    // const char *ip = argv[1];
    int port = atoi(argv[1]);

    sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    // inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    int listenfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(listenfd >= 0);

    int ret = bind(listenfd, (sockaddr*)&address, sizeof(address));
    assert(ret != -1);
    ret = listen(listenfd, 5);
    assert(ret != -1);

    epoll_event events[MAX_EVENT_NUMBER];
    int epollfd = epoll_create(MAX_EVENT_NUMBER);
    assert(epollfd != -1);
    /*
     * 监听套接字listenfd上不能注册EPOLLONESHOT事件，否则应用程序只能处理一个客户连接
     * 因为后续的客户连接请求将不再触发listenfd上的EPOLLIN事件
     */
    addfd(epollfd, listenfd, false);

    while (true) {
        ret = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);        ////< @note 不设置超时
        if (ret < 0) {
            printf("epoll failure\n");
            break;
        }
        for (int i = 0; i < ret; ++i) {
            int sockfd = events[i].data.fd;
            if (sockfd == listenfd) {
                sockaddr_in client_address;
                socklen_t client_addr_length = sizeof(client_address);
                int connfd = accept(listenfd, (sockaddr*)&client_address, &client_addr_length);
                addfd(epollfd, connfd, true);                           ////< @note 客户端套接字设置oneshot
            } else if (events[i].events & EPOLLIN) {                    ////< @note 客户端发送了数据到服务器端
                pthread_t thread;
                fds fds_for_new_worker(epollfd, sockfd);
                pthread_create(&thread, nullptr, worker, (void*)&fds_for_new_worker);   ////< @note 启动一个工作线程为sockfd服务
            } else {
                printf("something else happened\n");
            }
        }
    }
    close(listenfd);
    return 0;
}