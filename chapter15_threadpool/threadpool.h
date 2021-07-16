//
// Created by zhuque on 2021/4/18.
//

#ifndef HIGHPERFORMANCELINUXSERVER_THREADPOOL_H
#define HIGHPERFORMANCELINUXSERVER_THREADPOOL_H

#include <list>
#include <cstdio>
#include <exception>
#include <pthread.h>
#include "locker.h"

////< @brief 线程池类，将它定义为模板类是为了代码复用，模板参数T是任务类
template<typename T>
class threadpool {
private:
    int m_thread_number;        ////< @param 线程池中的线程数
    int m_max_requests;         ////< @param 请求队列中允许的最大请求数
    pthread_t *m_threads;       ////< @param 描述线程池的数组，其大小为m_thread_number
    std::list<T*> m_workqueue;  ////< @param 请求队列
    locker m_queuelocker;       ////< @param 保护请求队列的互斥锁
    sem m_queuestat;            ////< @param 是否有任务需要处理
    bool m_stop;                ////< @param 是否结束线程

public:
    ////< @brief 参数thread_number是线程池中线程的数量，max_requests是请求队列中最多允许的、等待处理的请求数量
    threadpool(int thread_number=0, int max_requests=10000);
    ~threadpool();
    ////< @brief 往请求队列中添加任务
    bool append(T *request);
    ////< @brief 工作线程运行的函数，它不断从工作队列中取出任务并执行
    static void* worker(void *arg);
    void run();
};

template<typename T>
threadpool<T>::threadpool(int thread_number, int max_requests) : m_thread_number(thread_number),
                                                                 m_max_requests(max_requests), m_stop(false),
                                                                 m_threads(nullptr) {
    if (thread_number <= 0 || max_requests <= 0) {
        throw std::exception();
    }
    m_threads = new pthread_t[m_thread_number];
    if (!m_threads) {
        throw std::exception();
    }

    ////< @note 创建thread_number个线程，并将它们都设置为脱离线程
    for (int i = 0; i < thread_number; ++i) {
        printf("create the %dth thread\n", i);
        if (pthread_create(m_threads + i, nullptr, worker, this) != 0) {
            delete [] m_threads;
            throw std::exception();
        }
        if (pthread_detach(m_threads[i])) {
            delete [] m_threads;
            throw std::exception();
        }
    }
}

template<typename T>
threadpool<T>::~threadpool() {
    delete [] m_threads;
    m_stop = true;
}

template<typename T>
bool threadpool<T>::append(T *request) {
    ////< @note 操作工作队列时一定要加锁，因为它被所有线程共享
    m_queuelocker.lock();
    if (m_workqueue.size() > m_max_requests) {
        m_queuelocker.unlock();
        return false;
    }
    m_workqueue.push_back(request);
    m_queuelocker.unlock();
    m_queuestat.post();         ////< @attention 为什么使用post，这个信号量在记录工作队列中有多少待处理消息
    return true;
}

template<typename T>
void* threadpool<T>::worker(void *arg) {
    threadpool *pool = (threadpool*)arg;            ////< @note 这个arg就是L52传进来的this
    pool->run();                ////< @attention 注意这个run的作用
    return pool;
}

template<typename T>
void threadpool<T>::run() {
    while (!m_stop) {
        /*
         * 这里一定是信号量和互斥量共同保护工作队列
         * 信号量的值代表了工作队列中待处理的消息数量
         * 若仅有信号量没有互斥量，当工作队列有多个消息时多个线程可能取得同一消息
         * 若仅有互斥量没有信号量，则会和append函数发生互斥量的竞争降低线程效率
         */
        m_queuestat.wait();
        m_queuelocker.lock();
        if (m_workqueue.empty()) {
            m_queuelocker.unlock();
            continue;
        }
        T *request = m_workqueue.front();
        m_workqueue.pop_front();
        m_queuelocker.unlock();
        if (!request) {
            continue;
        }
        request->process();
    }
}

#endif//HIGHPERFORMANCELINUXSERVER_THREADPOOL_H
