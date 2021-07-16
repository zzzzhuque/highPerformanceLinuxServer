//
// Created by zhuque on 2021/3/9.
//
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <csignal>
#include <unistd.h>
#include <cstdlib>
#include <cassert>
#include <cstdio>
#include <cstring>

static bool stop = false;

static void handleTerm(int sig) {                   ////< @note SIGTERM信号的处理函数，触发时结束主程序中的循环
    stop = true;
}

int main(int argc, char *argv[]) {
    signal(SIGTERM, handleTerm);

    if (argc <= 3) {
        printf("Usage: %s ip address port number backlog\n", argv[0]);
        return 1;
    }
    const char *ip = argv[1];
    int port = std::atoi(argv[2]);
    int backlog = std::atoi(argv[3]);

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    assert(sock >= 0);
    sockaddr_in address;
    bzero(&address, sizeof(address));               ////< @note 与memset作用相同
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    int ret = bind(sock, (sockaddr*)&address, sizeof(address));
    assert(ret != -1);

    ret = listen(sock, backlog);
    assert(ret != -1);

    while (!stop) {                                 ////< @note 循环等待连接，直到SIGTERM信号将它中断
        sleep(1);
    }

    close(sock);                                    ////< @note 关闭socket
    return 0;
}