//
// Created by zhuque on 2021/3/26.
//

#ifndef HIGHPERFORMANCELINUXSERVER_TIMELST_H
#define HIGHPERFORMANCELINUXSERVER_TIMELST_H

#include <ctime>
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
#include <pthread.h>
#include <sys/types.h>
#include <sys/epoll.h>

#define BUFFER_SIZE 64

class util_timer;           ////< @note 前项声明

////< @brief 用户数据结构，客户端socket地址、socket文件描述符、读缓存和定时器
struct client_data {
    sockaddr_in address;
    int sockfd;
    char buf[BUFFER_SIZE];
    util_timer *timer;
};

////< @brief 定时器类
class util_timer {
public:
    time_t expire;                      ////< @param 任务的超时时间，这里使用绝对时间
    void (*cb_func)(client_data*);      ////< @param 任务回调函数
    ////< @brief 回掉函数处理的客户数据，由定时器的执行者传递给回掉函数
    client_data *user_data;             ////< @param
    util_timer *prev;                   ////< @param 指向前一个定时器
    util_timer *next;                   ////< @param 指向下一个定时器

public:
    util_timer() : prev(nullptr), next(nullptr) {}
};

////< @brief 定时器链表。它是一个升序、双向链表，且带有头节点和尾节点
class sort_timer_lst {
private:
    util_timer *head;
    util_timer *tail;

public:
    sort_timer_lst() : head(nullptr), tail(nullptr) {}

    ////< @brief 链表被销毁时，删除其中所有定时器
    ~sort_timer_lst() {
        util_timer *tmp = head;
        while (tmp) {
            head = tmp->next;
            delete tmp;
            tmp = head;
        }
    }

    ////< @brief 将目标定时器timer添加到链表中
    void add_timer(util_timer *timer) {
        if (!timer) {
            return;
        }
        if (!head) {                    ////< @note 若链表为空，则头尾节点指向同一个定时器
            head = timer;
            tail = timer;
        }
        /*
         * 如果目标定时器的超时时间小于当前链表中所有定时器的超时时间
         * 则把该定时器插入链表头部，作为链表新的头节点
         * 否则就需要调用add_timer的重载函数，把它插入链表中合适的位置
         * 以保证链表的升序特性
         */
        if (timer->expire < head->expire) {
            timer->next = head;
            head->prev = timer;
            head = timer;
            return;
        }
        add_timer(timer, head);         ////< @note 函数重载
    }

    /*
     * 当某个定时任务发生变化时，调整对应的定时器在链表中的位置
     * 这个函数之考虑被调整的定时器的超时时间延长情况，即该定时器需要往链表的尾部移动
     */
    void adjust_timer(util_timer *timer) {
        if (!timer) {
            return;
        }
        util_timer *tmp = timer->next;
        /*
         * 如果被调整的目标定时器处在链表尾部
         * 或者该定时器新的超时值仍然小于其下一个定时器的超时值，则不作调整
         */
        if (!tmp || (timer->expire < tmp->expire)) {
            return;
        }
        ////< @note 如果定时器是链表的头节点，则将该定时器从链表中取出并重新插入链表
        if (timer == head) {
            head = head->next;
            head->prev = nullptr;
            timer->next = nullptr;
            add_timer(timer, head);
        } else {
            /*
             * 如果目标定时器不是链表的头节点，则将该定时器从链表取出
             * 然后插入其原来所在位置之后的部分链表中
             */
            timer->prev->next = timer->next;
            timer->next->prev = timer->prev;
            add_timer(timer, timer->next);
        }
    }

    ////< @brief 将目标定时器timer从链表中删除
    void del_timer(util_timer *timer) {
        if (!timer) {
            return;
        }
        ////< @note 下面这个条件成立表示链表中只有一个定时器，即目标定时器
        if (timer == head && timer == tail) {
            delete timer;
            head = nullptr;
            tail = nullptr;
            return;
        }
        /*
         * 如果链表中至少有两个定时器，且目标定时器是链表的头节点
         * 则将链表的头节点重置为原头节点的下一个节点，然后删除目标定时器
         */
        if (timer == head) {
            head = head->next;
            head->prev = nullptr;
            delete timer;
            return;
        }
        /*
         * 如果链表中至少有两个定时器，且目标定时器是链表的尾节点
         * 则将链表的尾节点重置为原尾节点的前一个节点，然后删除目标定时器
         */
        if (timer == tail) {
            tail = tail->prev;
            tail->next = nullptr;
            delete timer;
            return;
        }
        /*
         * 如果目标定时器位于链表的中间，则把它前后的定时器串联起来
         * 然后删除目标定时器
         */
        timer->prev->next = timer->next;
        timer->next->prev = timer->prev;
        delete timer;
        return;
    }

    /*
     * SIGALRM信号每次被触发就在其信号处理函数（如果使用统一事件源，则是主函数）
     * 中执行一次tick函数，以处理链表上到期的任务
     */
    void tick() {                       ////< @note 链表为空，不作处理
        if (!head) {
            return;
        }
        printf("timer tick\n");
        time_t cur = time(nullptr);     ////< @note 获得系统当前的时间
        util_timer *tmp = head;
        ////< @attention 从头节点开始依次处理每个定时器，直到遇到一个尚未到期的定时器，这就是定时器的核心逻辑
        while (tmp) {
            /*
             * 因为每个定时器都使用绝对时间作为超时值，所以我们可以对定时器的超时值和系统当前时间
             * 进行比较，判断定时器是否到期
             */
            if (cur < tmp->expire) {
                break;
            }
            ////< @note 调用定时器的回调函数
            tmp->cb_func(tmp->user_data);
            ////< @note 执行完定时器中的定时任务后，就将他从链表中删除，并重置链表头节点
            head = tmp->next;
            if (head) {
                head->prev = nullptr;
            }
            delete tmp;
            tmp = head;
        }
    }

private:
    /*
     * 一个重载的辅助函数，它被公有的add_timer函数和adjust_timer函数调用
     * 该函数表示将目标定时器timer添加到节点lst_head之后的部分链表中
     */
    void add_timer(util_timer *timer, util_timer *lst_head) {
        util_timer *prev = lst_head;
        util_timer *tmp = prev->next;
        /*
         * 遍历lst_head节点之后的部分链表，直到找到一个超时时间大于目标定时器的超时时间的节点
         * 并将目标定时器插入该节点之前
         */
        while (tmp) {
            if (timer->expire < tmp->expire) {
                prev->next = timer;
                timer->next = tmp;
                tmp->prev = timer;
                timer->prev = prev;
                break;
            }
            prev = tmp;
            tmp = tmp->next;
        }
        /*
         * 如果遍历完lst_head节点之后的部分链表，仍未找到超时时间大于目标定时器的超时时间的节点
         * 则将目标定时器插入链表尾部，并将它设置为链表新的尾节点
         */
        if (!tmp) {
            prev->next = timer;
            timer->prev = prev;
            timer->next = nullptr;
            tail = timer;
        }
    }
};

#endif//HIGHPERFORMANCELINUXSERVER_TIMELST_H
