//
// Created by zhuque on 2021/3/23.
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

    int listenfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(listenfd >= 0);
    ret = bind(listenfd, (sockaddr*)&address, sizeof(address));
    assert(ret != -1);
    ret = listen(listenfd, 5);
    assert(ret != -1);

    sockaddr_in client_address;
    socklen_t client_addrlength = sizeof(client_address);
    int connfd = accept(listenfd, (sockaddr*)&client_address, &client_addrlength);
    if (connfd < 0) {
        printf("errno is: %d\n", errno);
        close(listenfd);
        return 1;
    }

    char buf[1024];
    fd_set read_fds;
    fd_set exception_fds;
    FD_ZERO(&read_fds);
    FD_ZERO(&exception_fds);

    while (true) {
        memset(buf, '\0', sizeof(buf));
        FD_SET(connfd, &read_fds);
        FD_SET(connfd, &exception_fds);
        ret = select(connfd+1, &read_fds, nullptr, &exception_fds, nullptr);
        if (ret < 0) {
            printf("selection failed\n");
            break;
        }
        if (FD_ISSET(connfd, &read_fds)) {          ////< @note 对于可读事件，采用普通的recv函数读取数据
            ret = recv(connfd, buf, sizeof(buf)-1, 0);
            if (ret < 0) {
                break;
            }
            printf("get %d bytes of normal data: %s\n", ret, buf);
            sleep(1);
        } else if (FD_ISSET(connfd, &exception_fds)) {      ////< @note 对于异常事件，采用带MSG_OOB标志的recv函数读取带外数据
            ret = recv(connfd, buf, sizeof(buf)-1, MSG_OOB);
            if (ret <= 0) {
                break;
            }
            printf("get %d bytes of oob data: %s\n", ret, buf);
        }
    }
    close(connfd);
    close(listenfd);
    return 0;
}