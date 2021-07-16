//
// Created by zhuque on 2021/4/8.
//

#ifndef HIGHPERFORMANCELINUXSERVER_PROCESSPOOL_H
#define HIGHPERFORMANCELINUXSERVER_PROCESSPOOL_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cassert>
#include <cstdio>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <cstdlib>
#include <sys/epoll.h>
#include <csignal>
#include <sys/wait.h>
#include <sys/stat.h>

////< @brief 描述一个子进程的类
class process {
public:
    pid_t m_pid;                                    ////< @param 目标子进程的PID
    int m_pipefd[2];                                ////< @param 储存套接字对，用于进程通信

public:
    process() : m_pid(-1) {}
};

/*
 * 进程池类，将它定义为模板类是为了代码复用
 * 其模板参数是处理逻辑任务的类
 */
template<typename T>
class processpool {
private:
    static const int MAX_PROCESS_NUMBER = 16;       ////< @param 进程池允许的最大进程数量
    static const int USER_PER_PROCESS = 65536;      ////< @param 每个子进程最多能处理的客户数量
    static const int MAX_EVENT_NUMBER = 10000;      ////< @param epoll最多能处理的事件数
    int m_process_number;                           ////< @param 进程池中的进程总数
    int m_idx;                                      ////< @param 子进程在池中的序号，从0开始
    int m_epollfd;                                  ////< @param 每个进程都有一个epoll内核事件表，用m_epollfd标识
    int m_listenfd;                                 ////< @param 监听socket
    int m_stop;                                     ////< @param 子进程通过m_stop来决定是否停止运行
    process *m_sub_process;                         ////< @param 保存所有子进程的描述信息
    static processpool<T> *m_instance;              ////< @param 进程池静态实例

private:
    ////< @brief 构造函数定义为私有的，因此我们只能通过后面的create静态函数来创建processpoll实例
    processpool(int listenfd, int process_number=0);
    void setup_sig_pipe();
    void run_parent();
    void run_child();

public:
    ////< @brief 单例模式，以保证程序最多创建一个processpool实例，这是程序正确处理信号的必要条件
    static processpool<T>* create(int listenfd, int process_number=0) {
        if (!m_instance) {
            m_instance = new processpool<T>(listenfd, process_number);
        }
        return m_instance;
    }

    ~processpool() {
        delete [] m_sub_process;                    ////< @attention 注意一下这个变量是不是指向一个数组，L132
    }

    ////< @brief 启动进程池
    void run();
};

template<typename T>
processpool<T>* processpool<T>::m_instance = nullptr;

/*
 * 用于处理信号的管道，以实现统一事件源，后面总称为信号管道
 * 本质是一对无名套接字，0是读套接字，1是写套接字
 */
static int sig_pipefd[2];

static int setnonblocking(int fd) {
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

static void addfd(int epollfd, int fd) {
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;               ////< @note 设置为边缘触发
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}

////< @brief 从epollfd标识的epoll内核事件表中删除fd上的所有注册事件
static void removefd(int epollfd, int fd) {
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
    close(fd);
}

static void sig_handler(int sig) {
    int save_errno = errno;
    int msg = sig;
    send(sig_pipefd[1], (char*)&msg, 1, 0);         ////< @note 通过套接字1向套接字0发送消息
    errno = save_errno;
}

static void addsig(int sig, void(handler)(int), bool restart=true) {
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = handler;
    if (restart) {
        sa.sa_flags |= SA_RESTART;
    }
    sigfillset(&sa.sa_mask);
    assert(sigaction(sig, &sa, nullptr) != -1);
}

/*
 * 进程池构造函数
 * 参数listenfd是监听socket，必须在创建进程池之前被创建，否则子进程无法直接引用它
 * 参数process_number指定进程池中子进程的数量
 * 使用for循环创建process_number个子进程，并建立它们和父进程之间的管道
 */
template<typename T>
processpool<T>::processpool(int listenfd, int process_number) :
        m_listenfd(listenfd), m_process_number(process_number), m_idx(-1), m_stop(false){
    assert((process_number > 0) && (process_number <= MAX_PROCESS_NUMBER));
    m_sub_process = new process[process_number];
    assert(m_sub_process);
    for (int i = 0; i < process_number; ++i) {
        int ret = socketpair(PF_UNIX, SOCK_STREAM, 0, m_sub_process[i].m_pipefd);
        assert(ret == 0);

        m_sub_process[i].m_pid = fork();
        assert(m_sub_process[i].m_pid >= 0);

        if (m_sub_process[i].m_pid > 0) {           ////< @note 处于主进程，关掉管道1的写功能，只保留读功能
            close(m_sub_process[i].m_pipefd[1]);    ////< @attention 关注一下父子进程在管道的读写情况，0-写，1-读
            continue;
        } else {                                    ////< @note 处于子进程，关掉管道0的读功能，只保留写功能
            close(m_sub_process[i].m_pipefd[0]);
            m_idx = i;
            break;
        }
    }
}

////< @brief 统一事件源，就是把描述符读写、信号等事件统一在epoll中监听
template<typename T>
void processpool<T>::setup_sig_pipe() {
    m_epollfd = epoll_create(5);                    ////< @note 创建epoll事件监听表和信号管道
    assert(m_epollfd != -1);

    /*
     * socketpair()函数用于创建一对无名的、相互连接的套接字
     * 参数：domain是Unix本地域协议族；sv[0]可读可写；sv[1]可读可写
     * 返回值：如果函数成功，则返回0，创建好的套接字分别是sv[0]和sv[1]；否则返回-1，错误码保存于errno中
     * 读、写操作可以位于同一个进程，也可以分别位于不同的进程。如果是父子进程，一般会功能分离，一个进程用来读，一个用来写
     * 因为文件描述符sv[0]和sv[1]是进程共享的，所以读的进程要关闭写描述符，写的进程关闭读描述符
     */
    int ret = socketpair(PF_UNIX, SOCK_STREAM, 0, sig_pipefd);
    assert(ret != -1);

    setnonblocking(sig_pipefd[1]);                  ////< @note 将套接字1设置为非阻塞
    addfd(m_epollfd, sig_pipefd[0]);                ////< @note 监听套接字0的读事件，且设置为非阻塞

    addsig(SIGCHLD, sig_handler);                   ////< @note 子进程状态发生变化（退出或暂停）
    addsig(SIGTERM, sig_handler);                   ////< @note 终止进程，kill命令默认发送的信号就是SIGTERM
    addsig(SIGINT, sig_handler);                    ////< @note 键盘输入以中断进程（Ctrl+C）
    addsig(SIGPIPE, SIG_IGN);                       ////< @note 往读端被关闭的管道或者socket连接中写数据，如果func参数被设置为SIG_IGN，该信号将被忽略
}

/*
 * 父进程中m_idx值为-1，子进程中m_idx大于等于0，我们据此判断接下来要运行的
 * 是父进程代码还是子进程代码
 */
template<typename T>
void processpool<T>::run() {
    if (m_idx != -1) {
        run_child();                                ////< @note 运行子进程
        return;
    }
    run_parent();                                   ////< @note 运行父进程
}

template<typename T>
void processpool<T>::run_child() {
    setup_sig_pipe();

    ////< @note 每个子进程都通过其在进程池中的序号值m_idx找到与父进程通信的管道
    int pipefd = m_sub_process[m_idx].m_pipefd[1];
    ////< @note 子进程需要监听管道文件描述符pipefd，因为父进程将通过它来通知子进程accept新连接
    addfd(m_epollfd, pipefd);

    epoll_event events[MAX_EVENT_NUMBER];
    T *users = new T [USER_PER_PROCESS];            ////< @attention 注意T的类别
    assert(users);
    int number = 0;
    int ret = -1;

    while (!m_stop) {
        number = epoll_wait(m_epollfd, events, MAX_EVENT_NUMBER, -1);
        if (number < 0 && errno != EINTR) {
            printf("epoll failure!\n");
            break;
        }

        for (int i = 0; i < number; ++i) {
            int sockfd = events[i].data.fd;
            if (sockfd == pipefd && (events[i].events & EPOLLIN)) {                 ////< @note 从父进程接收消息
                int client = 0;
                /*
                 * 从父子进程之间的套接字读取数据，并将结果保存在变量client中
                 * 如果读取成功，则表示新客户端连接到来
                 */
                ret = recv(sockfd, (char*)&client, sizeof(client), 0);              ////< @attention client中存放来自new_conn的内容，但是下面却没有用到这个变量
                if ((ret<0 && errno!=EAGAIN) || ret == 0) {                         ////< @note 读取失败或未读到消息
                    continue;
                } else {
                    sockaddr_in client_address;
                    socklen_t client_addrLength = sizeof(client_address);
                    int connfd = accept(m_listenfd, (sockaddr*)&client_address, &client_addrLength);
                    if (connfd < 0) {
                        printf("errno is: %d\n", errno);
                        continue;
                    }
                    addfd(m_epollfd, connfd);
                    /*
                     * 模板类T必须实现init方法，以初始化一个客户连接
                     * 我们直接使用connfd来索引逻辑处理对象（T类型的对象），以提高程序效率
                     */
                    users[connfd].init(m_epollfd, connfd, client_address);
                }
            } else if (sockfd == sig_pipefd[0] && (events[i].events & EPOLLIN)) {   ////< @note 产生信号
                int sig;
                char signals[1024];
                ret = recv(sig_pipefd[0], signals, sizeof(signals), 0);
                if (ret < 0) {
                    continue;
                } else {
                    for (int i = 0; i < ret; ++i) {
                        switch (signals[i]) {
                            case SIGCHLD : {        ////< @note 收到子进程状态发生变化的信号
                                pid_t pid;
                                int stat;
                                /*
                                 * 等待任何一个子进程退出，使用WNOHANG参数，即使没有子进程退出，也会立即返回，不会阻塞
                                 * 当正常返回的时候，waitpid返回收集到的子进程ID
                                 * 如果设置了WNOHANG，而调用中waitpid发现没有已退出的子进程可收集，则返回0
                                 * 如果调用中出错，则返回-1，这时errno会被设置成相应的值以指示错误所在
                                 * 当pid所指示的子进程不存在，或此进程存在，但不是调用进程的子进程，waitpid就会出错返回，这时errno被设置为ECHILD
                                 */
                                while ((pid=waitpid(-1, &stat, WNOHANG)) > 0) {
                                    continue;
                                }
                                break;
                            }
                            case SIGTERM :          ////< @note 收到进程终止信号
                            case SIGINT: {
                                m_stop = true;
                                break;
                            }
                            default : {
                                break;
                            }
                        }
                    }
                }
            } else if (events[i].events & EPOLLIN) {                                ////< @note 产生新的客户端请求，调用逻辑对象的process处理
                users[sockfd].process();
            } else {
                continue;
            }
        }
    }

    delete [] users;
    users = nullptr;
    close(pipefd);
    /*
     * close(m_listenfd);
     * 注意应该由m_listenfd的创建者来关闭这个文件描述符，即所谓的对象
     * （比如文件描述符、段内存）由哪个函数创建就应该由哪个函数销毁
     */
    close(m_epollfd);
}

template<typename T>
void processpool<T>::run_parent() {
    setup_sig_pipe();

    addfd(m_epollfd, m_listenfd);                   ////< @note 父进程监听m_listenfd

    epoll_event events[MAX_EVENT_NUMBER];
    int sub_process_counter = 0;
    int new_conn = 1;
    int number = 0;
    int ret = -1;

    while (!m_stop) {
        number = epoll_wait(m_epollfd, events, MAX_EVENT_NUMBER, -1);
        if (number < 0 && errno != EINTR) {
            printf("epoll failure\n");
            break;
        }
        for (int i = 0; i < number; ++i) {
            int sockfd = events[i].data.fd;
            if (sockfd == m_listenfd) {
                /*
                 * 如果有新连接到来，就采用round robin方式将其分配给一个子进程处理
                 * Round Robin（中文翻译为轮询调度）是一种轮询调度的方式
                 * 使用变量i、sub_process_counter、m_process_number通过取模的方式实现新连接依次给到不同进程
                 */
                int i = sub_process_counter;
                do {
                    if (m_sub_process[i].m_pid != -1) {                             ////< @attention m_pidfork之后，什么时候被重新赋值-1，L346
                        break;
                    }
                    i = (i+1) % m_process_number;
                } while (i != sub_process_counter);

                if (m_sub_process[i].m_pid == -1) { ////< @note 轮询一圈发现所有的子进程都已经停止，主进程也停止
                    m_stop = true;
                    break;
                }
                sub_process_counter = (i+1) % m_process_number;                     ////< @attention new_conn起什么作用
                send(m_sub_process[i].m_pipefd[0], (char*)&new_conn, sizeof(new_conn), 0);
                printf("send request to child %d\n", i);                            ////< @attention 一个进程分配了多个新连接如何处理
            } else if (sockfd == sig_pipefd[0] && (events[i].events & EPOLLIN)) {   ////< @note 处理父进程接收到的信号
                int sig;
                char signals[1024];
                ret = recv(sig_pipefd[0], signals, sizeof(signals), 0);
                if (ret <= 0) {
                    continue;
                } else {
                    for (int i = 0; i < ret; ++i) {
                        switch (signals[i]) {
                            case SIGCHLD : {
                                pid_t pid;
                                int stat;
                                while ((pid = waitpid(-1, &stat, WNOHANG)) > 0) {
                                    /*
                                     * 如果进程池中第i个子进程退出了，则主进程关闭相应的通信通道
                                     * 并设置相应的m_pid为-1，以标记该子进程已经退出
                                     */
                                    for (int i = 0; i < m_process_number; ++i) {
                                        if (m_sub_process[i].m_pid == pid) {
                                            printf("child %d join\n", i);
                                            close(m_sub_process[i].m_pipefd[0]);
                                            m_sub_process[i].m_pid = -1;
                                        }
                                    }
                                }
                                m_stop = true;      ////< @note 如果所有子进程都已经退出了，则父进程也退出
                                for (int i = 0; i < m_process_number; ++i) {
                                    if (m_sub_process[i].m_pid != -1) {
                                        m_stop = false;
                                    }
                                }
                                break;
                            }
                            case SIGTERM :
                            case SIGINT : {
                                /*
                                 * 如果父进程接收到终止信号，那么就杀死所有子进程，并等待它们全部结束
                                 * 当然，通知子进程结束更好的方法是向父子进程之间的通信管道发送特殊数据
                                 * 这个功能待实现
                                 */
                                printf("kill all the child now\n");
                                for (int i = 0; i < m_process_number; ++i) {
                                    int pid = m_sub_process[i].m_pid;
                                    if (pid != -1) {
                                        kill(pid, SIGTERM);
                                    }
                                }
                                break;
                            }
                            default : {
                                break;
                            }
                        }
                    }
                }
            } else {
                continue;
            }
        }
    }
    // close(m_listenfd);   ////< @note 由创建者关闭该文件描述符
    close(m_epollfd);
}

#endif//HIGHPERFORMANCELINUXSERVER_PROCESSPOOL_H
