//
// Created by zhuque on 2021/4/19.
//

#ifndef HIGHPERFORMANCELINUXSERVER_HTTP_CONN_H
#define HIGHPERFORMANCELINUXSERVER_HTTP_CONN_H

#include <unistd.h>
#include <csignal>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cassert>
#include <sys/stat.h>
#include <cstring>
#include <pthread.h>
#include <cstdio>
#include <cstdlib>
#include <sys/mman.h>
#include <cstdarg>
#include <cerrno>
#include "locker.h"

class http_conn {
public:
    static const int FILENAME_LEN = 200;                    ////< @param 文件名的最大长度
    static const int READ_BUFFER_SIZE = 2048;               ////< @param 读缓冲区的大小
    static const int WRITE_BUFFER_SIZE = 1024;             ////< @param 写缓冲区的大小
    enum METHOD {GET = 0, POST, HEAD, PUT, DELETE,          ////< @param HTTP请求方法，目前仅支持GET
                  TRACE, OPTIONS, CONNECT, PATCH};
    enum CHECK_STATE {CHECK_STATE_REQUESTLINE = 0,          ////< @param 解析客户请求时，主状态机所处的状态
                       CHECK_STATE_HEADER,
                       CHECK_STATE_CONTENT};
    enum HTTP_CODE {NO_REQUEST, GET_REQUEST, BAD_REQUEST,   ////< @param 服务器处理HTTP请求的可能结果
                     NO_RESOURCE, FORBIDDEN_REQUEST, FILE_REQUEST,
                     INTERNAL_ERROR, CLOSED_CONNECTION};
    enum LINE_STATUS {LINE_OK = 0, LINE_BAD, LINE_OPEN};    ////< @param 行的读取状态

    static int m_epollfd;                                   ////< @param 所有socket上的事件都被注册到同一个epoll内核事件表中，所以将epoll文件描述符设置为静态变量
    static int m_user_count;                                ////< @param 统计用户数量

private:
    int m_sockfd;                                           ////< @param 该HTTP连接的socket
    sockaddr_in m_address;                                  ////< @param 该HTTP连接的socket地址
    char m_read_buf[READ_BUFFER_SIZE];                      ////< @param 读缓冲区
    int m_read_idx;                                         ////< @param 标识读缓冲中已经读入的客户数据的最后一个字节的下一个位置
    int m_checked_idx;                                      ////< @param 当前正在分析的字符在读缓冲区的位置
    int m_start_line;                                       ////< @param 当前正在解析行的起始位置
    char m_write_buf[WRITE_BUFFER_SIZE];                    ////< @param 写缓冲区
    int m_write_idx;                                        ////< @param 写缓冲区中待发送的字节数

    CHECK_STATE m_check_state;                              ////< @param 主状态机当前所处的状态
    METHOD m_method;                                        ////< @param 请求方法

    char m_real_file[FILENAME_LEN];                         ////< @param 客户请求的目标文件的完整路径，其内容等于doc_root+m_url，doc_root是网站根目录
    char *m_url;                                            ////< @param 客户请求的目标文件的文件名
    char *m_version;                                        ////< @param HTTP协议版本号，目前仅支持HTTP/1.1
    char *m_host;                                           ////< @param 主机名
    int m_content_length;                                   ////< @param HTTP请求消息体长度
    bool m_linger;                                          ////< @param HTTP请求是否要求保持连接

    char *m_file_address;                                   ////< @param 客户请求的目标文件将mmap到内存的起始位置
    struct stat m_file_stat;                                ////< @param 目标文件的状态。通过它可以判断文件是否存在、是否为目录、是否可读，并获取文件大小等信息
    iovec m_iv[2];                                          ////< @param 采用writev执行写操作，所以定义下面两个变量，
    int m_iv_count;                                         ////< @param 被写内存块的数量

public:
    http_conn() {}
    ~http_conn() {}

public:
    void init(int sockfd, const sockaddr_in &addr);         ////< @brief 初始化新接受的连接
    void close_conn(bool real_close=true);                  ////< @brief 关闭连接
    void process();                                         ////< @brief 处理客户连接
    bool read();                                            ////< @brief 非阻塞读操作
    bool write();                                           ////< @brief 非阻塞写操作

private:
    void init();                                            ////< @brief 初始化连接
    HTTP_CODE process_read();                               ////< @brief 解析HTTP请求
    bool process_write(HTTP_CODE ret);                      ////< @brief 填充HTTP应答

    ////< @brief 下面这组函数被process_read调用以分析HTTP请求
    HTTP_CODE parse_request_line(char *text);
    HTTP_CODE parse_headers(char *text);
    HTTP_CODE parse_content(char *text);
    HTTP_CODE do_request();
    char* get_line() {return m_read_buf + m_start_line;}
    LINE_STATUS parse_line();

    ////< @brief 下面这组函数被process_write调用以填充HTTP应答
    void unmap();
    bool add_response(const char *format, ...);
    bool add_content(const char *content);
    bool add_status_line(int status, const char *title);
    bool add_headers(int content_len);
    bool add_content_length(int content_len);
    bool add_linger();
    bool add_blank_line();
};

#endif//HIGHPERFORMANCELINUXSERVER_HTTP_CONN_H
