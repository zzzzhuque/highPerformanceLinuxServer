//
// Created by zhuque on 2021/3/23.
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
#include <csignal>
#include <fcntl.h>

#define BUF_SIZE 1024

static int connfd;

////< @brief SIGURG信号的处理函数
void sig_urg(int sig) {
    printf("recv SIGURG!\n");
    int save_errno = errno;
    char buffer[BUF_SIZE];
    memset(buffer, '\0', BUF_SIZE);
    int ret = recv(connfd, buffer, BUF_SIZE-1, MSG_OOB);        ////< @note 接收带外数据
    printf("got %d bytes of oob data '%s'\n", ret, buffer);
    errno = save_errno;
}

//void addsig(int sig, void (*sig_handler)(int)) {
void addsig() {
    printf("set SIGURG handler!\n");
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = sig_urg;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    assert(sigaction(SIGURG, &sa, 0) != -1);
    // sa.sa_flags |= SA_RESTART;
    // sigfillset(&sa.sa_mask);
    // assert(sigaction(sig, &sa, nullptr) != -1);
}

int main(int argc, char *argv[]) {
    if (argc <= 2) {
        printf("Usage: %s ip address port number\n", argv[0]);
        return 1;
    }
    const char *ip = argv[1];
    int port = atoi(argv[2]);

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
    socklen_t client_addrlength = sizeof(client);
    connfd = accept(sock, (sockaddr*)&client, &client_addrlength);

    // addsig(SIGURG, sig_urg);
    addsig();
    fcntl(connfd, F_SETOWN, getpid());      ////< @note 使用SIGURG信号之前，我们必须设置socket的宿主进程或进程组
    if (connfd < 0) {
        printf("errno is: %d\n", errno);
    } else {
        char buffer[BUF_SIZE];
        while (true) {
            memset(buffer, '\0', BUF_SIZE);
            sleep(1);
            ret = recv(connfd, buffer, BUF_SIZE-1, 0);
            if (ret <= 0) {
                break;
            }
            printf("get %d bytes of normal data '%s'\n", ret, buffer);

//            memset(buffer, '\0', BUF_SIZE);
//            ret = recv(connfd, buffer, BUF_SIZE-1, MSG_OOB);
//            if (ret <= 0) {
//                break;
//            }
//            printf("get %d bytes of oob data '%s'\n", ret, buffer);
        }

        close(connfd);
    }
    close(sock);
    return 0;
}