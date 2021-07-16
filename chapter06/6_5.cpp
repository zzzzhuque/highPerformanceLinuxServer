//
// Created by zhuque on 2021/3/13.
//
#include <cassert>
#include <cstdio>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <file>\n", argv[0]);
        return 1;
    }
    int filefd = open(argv[1], O_CREAT | O_WRONLY | O_TRUNC, 0666);
    assert(filefd > 0);

    int pipefd_stdout[2];
    int ret = pipe(pipefd_stdout);
    assert(ret != -1);

    int pipefd_file[2];
    ret = pipe(pipefd_file);
    assert(ret != -1);

    ////< @note 将标准输入内容移动管道pipefd_stdout
    ret = splice(STDIN_FILENO, nullptr, pipefd_stdout[1], nullptr, 32768,
                 SPLICE_F_MORE | SPLICE_F_MOVE);
    assert(ret != -1);
    ////< @note 将管道pipefd_stdout的输出复制到管道pipefd_file的输入端
    ret = tee(pipefd_stdout[0], pipefd_file[1], 32768, SPLICE_F_NONBLOCK);
    assert(ret != -1);
    ////< @note 将管道pipefd_file的输出定向到文件描述符filefd上，从而将标准输入的内容写入文件
    ret = splice(pipefd_file[0], nullptr, filefd, nullptr, 32768, SPLICE_F_MORE | SPLICE_F_MOVE);
    assert(ret != -1);
    ////< @note 将管道pipefd_stdout的输出定向到标准输出，其内容和写入文件的内容完全一致
    ret = splice(pipefd_stdout[0], nullptr, STDOUT_FILENO, nullptr, 32768, SPLICE_F_MORE | SPLICE_F_MOVE);
    assert(ret != -1);

    close(filefd);
    close(pipefd_stdout[0]);
    close(pipefd_stdout[1]);
    close(pipefd_file[0]);
    close(pipefd_file[1]);
    return 0;
}