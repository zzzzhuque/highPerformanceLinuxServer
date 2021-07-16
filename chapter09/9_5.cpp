//
// Created by zhuque on 2021/3/21.
//
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <cassert>
#include <cstdio>
#include <ctime>
#include <cerrno>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cstring>

#define BUFFER_SIZE 1023

int setnonblocking(int fd) {
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

/*
 * 超时连接函数，参数分别是服务器IP地址、端口号和超时时间（毫秒）
 * 函数成功时返回已经处于连接状态的socket，失败则返回-1
 */
int unblock_connect(const char *ip, int port, int time) {
    int ret = 0;
    sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    int fdopt = setnonblocking(sockfd);         ////< @note 设置为非阻塞
    ret = connect(sockfd, (sockaddr*)&address, sizeof(address));
    if (ret == 0) {                             ////< @note 如果连接成功，则恢复sockfd的属性，并立即返回之
        printf("connect with server immediately\n");
        fcntl(sockfd, F_SETFL, fdopt);
        return sockfd;
    } else if (errno != EINPROGRESS) {          ////< @note 如果连接没有立即建立，那么只有当errno是EINPROGRESS时才表示连接还在进行，否则出错返回
        printf("unblock connect net support\n");
        return -1;
    }

    fd_set readfds;
    fd_set writefds;
    timeval timeout;

    FD_ZERO(&readfds);
    FD_SET(sockfd, &writefds);

    timeout.tv_sec = time;
    timeout.tv_usec = 0;

    /*
     * 连接没有马上建立，等到10秒
     * 若在10秒内建立完成，则select可以检测到，否则连接失败
     */
    ret = select(sockfd+1, nullptr, &writefds, nullptr, &timeout);
    if (ret <= 0) {                             ////< @note select超时或出错，立即返回
        printf("connection time out\n");
        close(sockfd);
        return -1;
    }

    if (!FD_ISSET(sockfd, &writefds)) {
        printf("no events on sockfd found\n");
        close(sockfd);
        return -1;
    }

    int error = 0;
    socklen_t length = sizeof(error);
    if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &length) < 0) {
        printf("get socket option failed\n");   ////< @note 调用getsockopt来获取并清除sockfd上的错误
        close(sockfd);
        return -1;
    }
    if (error != 0) {                           ////< @note 错误号不为0表示连接出错
        printf("connection failed after select with the error: %d\n", error);
        close(sockfd);
        return -1;
    }
    printf("connection ready after select with the socket: %d\n", sockfd);
    fcntl(sockfd, F_SETFL, fdopt);
    return sockfd;
}

int main(int argc, char *argv[]) {
    if (argc <= 2) {
        printf("Usage: %s ip address port number\n", argv[0]);
        return -1;
    }
    const char *ip = argv[1];
    int port = atoi(argv[2]);

    int sockfd = unblock_connect(ip, port, 10);
    if (sockfd < 0) {
        return 1;
    }
    close(sockfd);
    return 0;
}