//
// Created by zhuque on 2021/3/14.
//
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>

#define BUF_SIZE 1024

int main() {
    char cwd[BUF_SIZE];
    auto ptr = getcwd(cwd, BUF_SIZE);
    printf("current root path is: %s \n", ptr);

    const char path[] = "/home/lht/.zhuque/highPerformanceLinuxServer/chapter07";
    chroot(path);
    ptr = getcwd(cwd, BUF_SIZE);
    printf("changed root path is: %s \n", ptr);
    return 0;
}