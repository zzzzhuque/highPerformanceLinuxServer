//
// Created by zhuque on 2021/4/14.
//
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <sys/epoll.h>
#include <csignal>
#include <sys/wait.h>
#include <sys/stat.h>

#include "processpool.h"

////< @brief 用于处理客户CGI请求的类，作为processpool类的模板参数
class cgi_conn {
private:
    static const int BUFFER_SIZE = 1024;    ////< @param 读缓冲区的大小
    static int m_epollfd;
    int m_sockfd;
    sockaddr_in m_address;
    char m_buf[BUFFER_SIZE];
    int m_read_idx;                         ////< @param 标记读缓冲中已经读入的客户数据的最后一个字节的下一个位置

public:
    cgi_conn() {}
    ~cgi_conn() {}

    ////< @brief 初始化客户连接，清空读缓冲区
    void init(int epollfd, int sockfd, const sockaddr_in &client_adr) {
        m_epollfd = epollfd;
        m_sockfd = sockfd;
        m_address = client_adr;
        memset(m_buf, '\0', BUFFER_SIZE);
        m_read_idx = 0;
    }

    void process() {
        int idx = 0;
        int ret = -1;
        while (true) {                      ////< @note 循环读取和分析客户数据
            idx = m_read_idx;
            ret = recv(m_sockfd, m_buf+idx, BUFFER_SIZE-1-idx, 0);
            if (ret < 0) {                  ////< @note 如果读操作发生错误，则关闭客户连接，但若是暂时无数据可读，则退出循环
                if (errno != EAGAIN) {
                    removefd(m_epollfd, m_sockfd);
                }
                break;
            } else if (ret == 0) {          ////< @note 如果对方关闭连接，则服务器也关闭连接
                removefd(m_epollfd, m_sockfd);
                break;
            } else {
                m_read_idx += ret;
                printf("user content is: %s\n", m_buf);
                for (; idx < m_read_idx; ++idx) {           ////< @note 如果遇到字符"\r\n"，则开始处理客户请求
                    if (idx >= 1 && m_buf[idx-1] == '\r' && m_buf[idx] == '\n') {
                        break;
                    }
                }
                if (idx == m_read_idx) {    ////< @note 如果没有遇到字符"\r\n"，则需要读取更多客户数据
                    continue;
                }
                m_buf[idx-1] = '\0';

                char *file_name = m_buf;
                /*
                 * 头文件：<unistd.h>
                 * 函数原型：int access(const char *pathname, int mode)
                 * 参数：pathname为文件路径或目录路径 mode为访问权限，F_OK检查文件是否存在
                 * 返回值：如果文件具有指定的访问权限，则函数返回0；如果文件不存在或者不能访问指定的权限，则返回-1
                 */
                if (access(file_name, F_OK) == -1) {        ////< @note 判断客户要运行的CGI程序是否存在
                    removefd(m_epollfd, m_sockfd);
                    break;
                }

                ret = fork();               ////< @note 创建子进程来执行CGI程序
                if (ret == -1) {
                    removefd(m_epollfd, m_sockfd);
                    break;
                } else if (ret > 0) {       ////< @note 父进程只需关闭连接
                    removefd(m_epollfd, m_sockfd);
                    break;
                } else {                    ////< @note 子进程将标准输出定向到m_sockfd，并执行CGI程序
                    close(STDOUT_FILENO);
                    dup(m_sockfd);
                    /*
                     * 头文件：  #include <unistd.h>
                     * 函数定义：int execl(const char *path, const char *arg, ...)
                     * 函数说明：execl()用来执行参数path所代表的文件，接下来的参数代表执行该文件时传递的argv[0], argv[1]...
                     *          最后一个参数必须用空指针nullptr作结束
                     * 返回值 ： 成功则不返回值，失败返回-1，失败原因存于errno中
                     */
                    execl(m_buf, m_buf, 0); ////< @note 也就是说这个可执行文件产生的输出会发送到客户端
                    exit(0);                ////< @note 退出这个临时子进程
                }
            }
        }
    }
};

int cgi_conn::m_epollfd = -1;

int main(int argc, char *argv[]) {
    if (argc <= 2) {
        printf("usage: %s ip address port number\n", argv[0]);
        return 1;
    }
    const char *ip = argv[1];
    int port = atoi(argv[2]);

    int listenfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(listenfd >= 0);

    int ret = 0;
    sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    ret = bind(listenfd, (sockaddr*)&address, sizeof(address));
    assert(ret != -1);

    ret = listen(listenfd, 5);
    assert(ret != -1);

    processpool<cgi_conn> *pool = processpool<cgi_conn>::create(listenfd, 5);
    ////< @attention 到这里开始就要区分主进程和子进程
    if (pool) {
        pool->run();
        delete pool;
    }
    close(listenfd);                        ////< @note 正如前文提到的，main函数创建了文件描述符listenfd，就由main函数关闭
    return 0;
}