//
// Created by zhuque on 2021/3/29.
//

#ifndef HIGHPERFORMANCELINUXSERVER_TIME_WHEEL_TIMER_H
#define HIGHPERFORMANCELINUXSERVER_TIME_WHEEL_TIMER_H

#include <ctime>
#include <netinet/in.h>
#include <cstdio>

#define BUFFER_SIZE 64

class tw_timer;

////< @brief 绑定socket和定时器
struct client_data {
    sockaddr_in address;
    int sockfd;
    char buffer[BUFFER_SIZE];
    tw_timer *timer;
};

////< @brief 定时器类
class tw_timer {
public:
    int rotation;                   ////< @note 记录定时器在时间轮转多少圈后生效
    int time_slot;                  ////< @note 记录定时器属于时间轮上哪个槽(对应的链表，下同)
    void (*cb_func)(client_data*);  ////< @note 定时器回调函数
    client_data *user_data;         ////< @note 客户数据
    ////< @attention 定义成双向链表，方便节点的插入和删除
    tw_timer *next;                 ////< @note 指向下一个定时器
    tw_timer *prev;                 ////< @note 指向前一个定时器

public:
    tw_timer(int rot, int ts) : next(nullptr), prev(nullptr), rotation(rot), time_slot(ts) {}
};

class time_wheel {
private:
    static const int N = 60;        ////< @note 时间轮上槽的数目
    static const int SI = 1;        ////< @note 每1秒时间轮转动一次，取槽间隔为1秒
    tw_timer *slots[N];             ////< @note 时间轮的槽，其中每个元素指向一个定时器链表，链表无序
    int cur_slot;                   ////< @note 时间轮当前槽

public:
    ////< @brief 初始化每个槽的头节点
    time_wheel() : cur_slot(0) {
        for (int i = 0; i < N; ++i) {
            slots[i] = nullptr;
        }
    }

    ////< @brief 遍历每个槽，并销毁其中的定时器
    ~time_wheel() {
        for (int i = 0; i < N; ++i) {
            tw_timer *tmp = slots[i];
            while (tmp) {
                slots[i] = tmp->next;
                delete tmp;
                tmp = slots[i];
            }
        }
    }

    ////< @brief 根据定时值timeout创建一个定时器，并把它插入合适的槽中
    tw_timer* add_timer(int timeout) {
        if (timeout < 0) {
            return nullptr;
        }
        int ticks = 0;
        /*
         * 下面根据待插入定时器的超时值计算它将在时间轮转动多少个滴答后被触发
         * 并将该滴答数存储于变量ticks中。如果待插入定时器的超时值小于时间轮的槽间隔SI
         * 则将ticks向上折合为1，否则就将ticks向下折合为timeout/SI
         */
        if (timeout < SI) {
            ticks = 1;
        } else {
            ticks = timeout / SI;
        }
        int rotation = ticks / N;   ////< @note 计算待插入的定时器在时间轮转动多少圈后被触发
        int ts = (cur_slot + (ticks % N)) % N;          ////< @note 计算待插入的定时器应该被插入哪个槽中
        tw_timer *timer = new tw_timer(rotation, ts);   ////< @note 创建新的定时器，它在时间轮转动rotation圈之后被触发，且位于第ts个槽上
        ////< @note 如果第ts个槽中尚无任何定时器，则把新建的定时器插入其中，并将该定时器设置为该槽的头节点
        if (!slots[ts]) {
            printf("add timer, rotation is %d, ts is %d, cur_slot is %d\n", rotation, ts, cur_slot);
            slots[ts] = timer;
        } else {                    ////< @note 否则，将定时器插入第ts个槽中，也是作为头节点
            timer->next = slots[ts];
            slots[ts]->prev = timer;
            slots[ts] = timer;
        }
        return timer;
    }

    ////< @brief 删除目标定时器timer
    void del_timer(tw_timer *timer) {
        if (!timer) {
            return;
        }
        int ts = timer->time_slot;
        ////< @note slots[ts]是目标定时器所在槽的头节点，如果目标定时器就是该头节点，则需要重置第ts个槽的头节点
        if (timer == slots[ts]) {
            slots[ts] = slots[ts]->next;
            if (slots[ts]) {
                slots[ts]->prev = nullptr;
            }
            delete timer;
        } else {
            timer->prev->next = timer->next;
            if (timer->next) {
                timer->next->prev = timer->prev;
            }
            delete timer;
        }
    }

    ////< @brief SI时间到后，调用该函数，时间轮向前滚动一个槽的间隔
    void tick() {
        tw_timer *tmp = slots[cur_slot];                ////< @note 取得时间轮上当前槽的头节点
        printf("current slot is %d\n", cur_slot);
        while (tmp) {
            printf("tick the timer once\n");
            if (tmp->rotation > 0) {                    ////< @note 如果定时器的rotation大于0，则它在这一轮不起作用
                tmp->rotation--;
                tmp = tmp->next;
            } else {                                    ////< @note 否则说明定时器已经到期，于是执行定时任务，然后删除该定时器
                tmp->cb_func(tmp->user_data);
                if (tmp == slots[cur_slot]) {
                    printf("delete header in cur_slot\n");
                    slots[cur_slot] = tmp->next;
                    delete tmp;
                    if (slots[cur_slot]) {
                        slots[cur_slot]->prev = nullptr;
                    }
                    tmp = slots[cur_slot];
                } else {
                    tmp->prev->next = tmp->next;
                    if (tmp->next) {
                        tmp->next->prev = tmp->prev;
                    }
                    tw_timer *tmp2 = tmp->next;
                    delete tmp;
                    tmp = tmp2;
                }
            }
        }
        cur_slot = ++cur_slot & N;  ////< @note 更新时间轮的当前槽，以反映时间轮的转动
    }
};

#endif//HIGHPERFORMANCELINUXSERVER_TIME_WHEEL_TIMER_H
