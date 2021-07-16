//
// Created by zhuque on 2021/3/22.
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

#define MAX_EVENT_NUMBER 1024
#define TCP_BUFFER_SIZE 512
#define UDP_BUFFER_SIZE 1024

int setnonblocking(int fd) {
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

/*
 * 向epoll例程注册新的客户端
 * 设置为边缘触发
 * 设置为非阻塞套接字
 */
void addfd(int epollfd, int fd) {
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}

int main(int argc, char *argv[]) {
    if (argc <= 2) {
        printf("Usage: %s ip address port number\n", argv[0]);
        return 1;
    }
    const char *ip = argv[1];
    int port = atoi(argv[2]);

    int ret = 0;
    sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    ////< @note 创建TCP套接字，并将其绑定到端口port上
    int listenfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(listenfd >= 0);

    ret = bind(listenfd, (sockaddr*)&address, sizeof(address));
    assert(ret != -1);

    ret = listen(listenfd, 5);
    assert(ret != -1);

    ////< @note 创建UDP套接字，并将其绑定到端口port上
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);
    int udpfd = socket(PF_INET, SOCK_DGRAM, 0);
    assert(udpfd >= 0);

    ret = bind(udpfd, (sockaddr*)&address, sizeof(address));
    assert(ret != -1);

    epoll_event events[MAX_EVENT_NUMBER];
    int epollfd = epoll_create(MAX_EVENT_NUMBER);
    assert(epollfd != -1);
    ////< @note 注册TCP套接字和UDP套接字上的可读事件
    addfd(epollfd, listenfd);
    addfd(epollfd, udpfd);

    while (true) {
        int number = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
        if (number < 0) {
            printf("epoll wait failure\n");
            break;
        }
        for (int i = 0; i < number; ++i) {
            int sockfd = events[i].data.fd;
            if (sockfd == listenfd) {
                sockaddr_in client_address;
                socklen_t client_addrlength = sizeof(client_address);
                int connfd = accept(listenfd, (sockaddr*)&client_address, &client_addrlength);
                addfd(epollfd, connfd);
            } else if (sockfd == udpfd) {
                char buf[UDP_BUFFER_SIZE];
                memset(buf, '\0', UDP_BUFFER_SIZE);
                sockaddr_in client_address;
                socklen_t client_addrLength = sizeof(client_address);
                ret = recvfrom(udpfd, buf, UDP_BUFFER_SIZE-1, 0, (sockaddr*)&client_address, &client_addrLength);
                if (ret > 0) {              ////< @note 收到什么就发回去什么
                    sendto(udpfd, buf, UDP_BUFFER_SIZE-1, 0, (sockaddr*)&client_address, client_addrLength);
                }
            } else if (events[i].events & EPOLLIN) {
                char buf[TCP_BUFFER_SIZE];
                while (true) {
                    memset(buf, '\0', TCP_BUFFER_SIZE);
                    ret = recv(sockfd, buf, TCP_BUFFER_SIZE-1, 0);
                    if (ret < 0) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                            break;
                        }
                        close(sockfd);
                        break;
                    } else if (ret == 0) {
                        close(sockfd);
                    } else {
                        send(sockfd, buf, ret, 0);
                    }
                }
            } else {
                printf("something else happened\n");
            }
        }
    }
    close(listenfd);
    return 0;
}