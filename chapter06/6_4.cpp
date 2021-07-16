//
// Created by zhuque on 2021/3/12.
//
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cassert>
#include <cstdio>
#include <unistd.h>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    if (argc <= 2) {
        printf("Usage: %s ip address port number\n", argv[0]);
        return 1;
    }
    const char *ip = argv[1];
    int port = std::atoi(argv[2]);

    sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    assert(sock >= 0);

    int ret = bind(sock, (sockaddr *) &address, sizeof(address));
    assert(ret != -1);

    ret = listen(sock, 5);
    assert(ret != -1);

    sockaddr_in client;
    socklen_t clientAddrLength = sizeof(client);
    int connfd = accept(sock, (sockaddr *) &client, &clientAddrLength);
    if (connfd < 0) {
        printf("errno is: %d\n", errno);
    } else {
        int pipefd[2];
        ret = pipe(pipefd);             ////< @note 创建管道
        assert(ret != -1);
        ret = splice(connfd, nullptr, pipefd[1], nullptr, 32768,
                     SPLICE_F_MORE | SPLICE_F_MOVE);
        assert(ret != -1);              ////< @note 将connfd上流入的客户数据定向到管道中
        ret = splice(pipefd[0], nullptr, connfd, nullptr, 32768,
                           SPLICE_F_MORE | SPLICE_F_MOVE);
        assert(ret != -1);
        close(connfd);
    }
    close(sock);
    return 0;
}