//
// Created by zhuque on 2021/3/10.
//
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cstring>

int main(int argc, char *argv[]) {
    if (argc <= 2) {
        printf("Usage: %s ip address port number\n", argv[0]);
        return 1;
    }
    const char *ip = argv[1];
    int port = atoi(argv[2]);

    sockaddr_in serverAddress;
    bzero(&serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &serverAddress.sin_addr);
    serverAddress.sin_port = htons(port);

    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(sockfd >= 0);
    if (connect(sockfd, (sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        printf("connection failed\n");
    } else {
        const char *oobData = "abc";
        const char *normalData = "123";
        send(sockfd, normalData, strlen(normalData), 0);
        send(sockfd, oobData, strlen(oobData), MSG_OOB);
        send(sockfd, normalData, strlen(normalData), 0);
    }
    sleep(10);
    close(sockfd);
    return 0;
}
