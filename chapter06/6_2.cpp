//
// Created by zhuque on 2021/3/12.
//
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024

static const char *statusLine[2] = {"200 OK", "500 Internet server error"};

int main(int argc, char *argv[]) {
    if (argc <= 3) {
        printf("Usage: %s ip address port number filename\n", argv[0]);
        return 1;
    }
    const char *ip = argv[1];
    int port = atoi(argv[2]);
    const char *filename = argv[3];               ////< @note 将目标文件作为程序的第三个参数传入

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
        char headerBuf[BUFFER_SIZE];             ////< @note 用于保存HTTP应答的状态行、头部字段和一个空行的缓存区
        memset(headerBuf, '\0', BUFFER_SIZE);
        char *fileBuf;                           ////< @note 用于存放目标文件内容的应用程序缓存
        struct stat fileStat;                           ////< @note 用于获取目标文件的属性，比如是否为目录，文件大小等
        bool valid = true;                       ////< @note 记录目标文件是否是有效文件
        int len = 0;
        if (stat(filename, &fileStat) < 0) {     ////< @note 获取指定路径的文件信息并保存到stat结构体中
            valid = false;                       ////< @note 文件不存在
        } else {
            if (S_ISDIR(fileStat.st_mode)) {     ////< @note 目标文件是一个目录
                valid = false;
            } else if (fileStat.st_mode & S_IROTH){         ////< @note 当前用户有读取目标文件的权限
                int fd = open(filename, O_RDONLY);          ////< @note 动态分配缓存区fileBuf，并指定其大小为目标文件的大小，然后将目标文件读入缓存区fileBuf中
                fileBuf = new char[fileStat.st_size+1];
                memset(fileBuf, '\0', fileStat.st_size+1);
                if (read(fd, fileBuf, fileStat.st_size+1) < 0) {
                    valid = false;
                }
            } else {                             ////< @note 用户没有权限或其他错误
                valid = false;
            }
        }

        if (valid) {                             ////< @note 如果目标文件有效，则发送正常的HTTP应答
            ret = snprintf(headerBuf, BUFFER_SIZE-1, "%s %s\r\n", "HTTP/1.1", statusLine[0]);
            len += ret;                          ////< @note 将HTTP应答的状态行、"Content-Length"头部字段和一个空行依次加入headerBuf中
            ret = snprintf(headerBuf+len, BUFFER_SIZE-1-len, "Content-Length: %d\r\n", fileStat.st_size);
            len += ret;
            ret = snprintf(headerBuf+len, BUFFER_SIZE-1-len, "%s", "\r\n");
            iovec iv[2];                         ////< @note 利用writev将headerBuf和fileBuf的内容一并写出
            iv[0].iov_base = headerBuf;
            iv[0].iov_len = strlen(headerBuf);
            iv[1].iov_base = fileBuf;
            iv[1].iov_len = fileStat.st_size;
            ret = writev(connfd, iv, 2);
        } else {                                 ////< @note 如果目标文件无效，则通知客户端服务器发生了内部错误
            ret = snprintf(headerBuf, BUFFER_SIZE-1, "%s %s\r\n", "HTTP/1.1", statusLine[1]);
            len += ret;
            ret = snprintf(headerBuf+len, BUFFER_SIZE-1-len, "%s", "\r\n");
            send(connfd, headerBuf, strlen(headerBuf), 0);
        }
        close(connfd);
        delete [] fileBuf;
    }
    close(sock);
    return 0;
}