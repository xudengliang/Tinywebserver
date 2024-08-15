#ifndef LOCKER_H
#define LOCKER_H

#include <exception>
#include <pthread.h>
#include <semaphore.h>

class sem // 信号量互斥
{
public:
  sem() {
    if (sem_init(&m_sem, 0, 0) != 0) {
      throw std::exception();
    }
  }
  sem(int num) {

    // sem_init函数用于初始化一个未命名的信号量
    if (sem_init(&m_sem, 0, num) != 0) {
      throw std::exception();
    }
  }
  ~sem() { sem_destroy(&m_sem); }

  // sem_wait函数将以原子操作方式将信号量减一,信号量为0时,sem_wait阻塞
  bool wait() { return sem_wait(&m_sem) == 0; }

  // sem_post函数以原子操作方式将信号量加一,信号量大于0时,唤醒调用sem_post的线程
  bool post() { return sem_post(&m_sem) == 0; }

private:
  sem_t m_sem;
};

class locker // 加锁互斥
{
public:
  locker() {
    if (pthread_mutex_init(&m_mutex, NULL) != 0) {
      throw std::exception();
    }
  }
  ~locker() { pthread_mutex_destroy(&m_mutex); }
  bool lock() { return pthread_mutex_lock(&m_mutex) == 0; }
  bool unlock() { return pthread_mutex_unlock(&m_mutex) == 0; }
  pthread_mutex_t *get() { return &m_mutex; }

private:
  pthread_mutex_t m_mutex;
};

class cond // 条件变量互斥
{
public:
  cond() {
    if (pthread_cond_init(&m_cond, NULL) != 0) {
      // pthread_mutex_destroy(&m_mutex);
      throw std::exception();
    }
  }
  ~cond() { pthread_cond_destroy(&m_cond); }

  // pthread_cond_wait函数用于等待目标条件变量.
  // 该函数调用时需要传入 mutex参数(加锁的互斥锁) ,
  // 函数执行时,先把调用线程放入条件变量的请求队列,然后将互斥锁mutex解锁,
  // 当函数成功返回为0时,互斥锁会再次被锁上.
  // 也就是说函数内部会有一次解锁和加锁操作.
  bool wait(pthread_mutex_t *m_mutex) {
    int ret = 0;
    // pthread_mutex_lock(&m_mutex);
    ret = pthread_cond_wait(&m_cond, m_mutex);
    // pthread_mutex_unlock(&m_mutex);
    return ret == 0;
  }

  bool timewait(pthread_mutex_t *m_mutex, struct timespec t) {
    int ret = 0;
    // pthread_mutex_lock(&m_mutex);
    ret = pthread_cond_timedwait(&m_cond, m_mutex, &t);
    // pthread_mutex_unlock(&m_mutex);
    return ret == 0;
  }
  bool signal() { return pthread_cond_signal(&m_cond) == 0; }

  // pthread_cond_broadcast函数以广播的方式唤醒所有等待目标条件变量的线程
  bool broadcast() { return pthread_cond_broadcast(&m_cond) == 0; }

private:
  // static pthread_mutex_t m_mutex;
  pthread_cond_t m_cond;
};
#endif
