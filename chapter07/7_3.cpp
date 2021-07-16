//
// Created by zhuque on 2021/3/14.
//
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include <cstdlib>

bool daemonize() {
    pid_t pid = fork();         ////< @note 创建子进程，关闭父进程，使程序在后台运行
    if (pid < 0) {
        return false;
    } else if (pid > 0) {       ////< @note 父进程退出
        exit(0);
    }
    umask(0);                   ////< @note pid=0表示目前处于子进程
    pid_t sid = setsid();       ////< @note 创建新的回话，设置本进程为进程组的首领
    if (sid < 0) {
        return false;
    }

    if (chdir("/") < 0) {       ////< @note 切换工作目录
        return false;
    }

    close(STDIN_FILENO);        ////< @note 关闭标准输入设备、标准输出设备、标准错误输出设备
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    open("/dev/null", O_RDONLY);////< @note 关闭其他文件描述符并定向到/dev/null
    open("/dev/null", O_RDWR);
    open("/dev/null", O_RDWR);
    return true;
}

int main() {
    return 0;
}