//
// Created by zhuque on 2021/3/14.
//
#include <unistd.h>
#include <fcntl.h>

int main() {
    const char *path = "/home/lht/.zhuque/highPerformanceLinuxServer/chapter06";
    chdir(path);
    int fd = open("data.txt", O_CREAT | O_TRUNC, 0664);
    close(fd);

    path = "/home/lht/.zhuque/highPerformanceLinuxServer/chapter07";
    chdir(path);
     fd = open("data.txt", O_CREAT | O_TRUNC, 0664);
    close(fd);
    return 0;
}