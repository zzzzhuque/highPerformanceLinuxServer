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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>

int main(int argc, char *argv[]) {
    if (argc <= 3) {
        printf("Usage: %s ip address port number filename\n", argv[0]);
        return 1;
    }
    const char *ip = argv[1];
    int port = atoi(argv[2]);
    const char *filename = argv[3];

    int filefd = open(filename, O_RDONLY);
    assert(filefd > 0);
    struct stat statBuf;
    fstat(filefd, &statBuf);

    sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    assert(sock >= 0);

    int ret = bind(sock, (sockaddr*)&address, sizeof(address));
    assert(ret != -1);

    ret = listen(sock, 5);
    assert(ret != -1);

    sockaddr_in client;
    socklen_t clientAddrLength = sizeof(client);
    int connfd = accept(sock, (sockaddr*)&client, &clientAddrLength);
    if (connfd < 0) {
        printf("errno is: %d\n", errno);
    } else {
        sendfile(connfd, filefd, nullptr, statBuf.st_size);
        close(connfd);
    }
    close(sock);
    return 0;
}