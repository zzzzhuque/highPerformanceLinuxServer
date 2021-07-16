//
// Created by zhuque on 2021/4/15.
//

#ifndef HIGHPERFORMANCELINUXSERVER_LOCKER_H
#define HIGHPERFORMANCELINUXSERVER_LOCKER_H

#include <exception>
#include <pthread.h>
#include <semaphore.h>

////< @brief 封装信号量的类
class sem {
private:
    sem_t m_sem;

public:
    ////< @brief 创建并初始化信号量
    sem() {
        /*
         * m_sem为信号量结构体；
         * 第二个参数不为０时此信号量在进程间共享，否则只能为当前进程的所有线程共享；
         * 第三个参数给出了信号量的初始值
         */
        if (sem_init(&m_sem, 0, 0) != 0) {
            throw std::exception();         ////< @note 构造函数没有返回值，可以通过抛出异常来报告错误
        }
    }

    ////< @brief 销毁信号量
    ~sem() {
        sem_destroy(&m_sem);
    }

    ////< @brief 等待信号量
    bool wait() {
        return sem_wait(&m_sem) == 0;
    }

    ////< @brief 增加信号量
    bool post() {
        return sem_post(&m_sem) == 0;
    }
};

////< @brief 封装互斥锁的类
class locker {
private:
    pthread_mutex_t m_mutex;

public:
    ////< @brief 创建并初始化互斥锁
    locker() {
        if (pthread_mutex_init(&m_mutex, nullptr) != 0) {
            throw std::exception();
        }
    }

    ////< @brief 销毁互斥锁
    ~locker() {
        pthread_mutex_destroy(&m_mutex);
    }

    ////< @brief 获取互斥锁
    bool lock() {
        return pthread_mutex_lock(&m_mutex) == 0;
    }

    ////< @brief 释放互斥锁
    bool unlock() {
        return pthread_mutex_unlock(&m_mutex) == 0;
    }
};

////< @brief 封装条件变量的类
class cond {
private:
    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;

public:
    ////< @brief 创建并初始化条件变量
    cond() {
        if (pthread_mutex_init(&m_mutex, nullptr) != 0) {
            throw std::exception();
        }
        ////< @note 构造函数中一旦出现问题，就应该立即释放已经成功分配了的资源
        if (pthread_cond_init(&m_cond, nullptr) != 0) {
            pthread_mutex_destroy(&m_mutex);
            throw std::exception();
        }
    }

    ////< @brief 销毁条件变量
    ~cond() {
        pthread_mutex_destroy(&m_mutex);
        pthread_cond_destroy(&m_cond);
    }

    ////< @brief 等待条件变量
    bool wait() {
        int ret = 0;
        pthread_mutex_lock(&m_mutex);
        ret = pthread_cond_wait(&m_cond, &m_mutex);
        pthread_mutex_unlock(&m_mutex);
        return ret == 0;
    }

    ////< @brief 唤醒等待条件变量的线程
    bool signal() {
        return pthread_cond_signal(&m_cond) == 0;
    }
};

#endif//HIGHPERFORMANCELINUXSERVER_LOCKER_H
