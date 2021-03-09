#ifndef threadpool_HPP
#define threadpool_HPP

#include <list>
#include <cstdio>
#include <exception>
#include <pthread.h>
#include <memory>
#include "locker.h"
template <typename T>
class threadpool
{
public:
    threadpool(int thread_number = 8, int max_requests = 10000);
    ~threadpool();
    bool append(std::shared_ptr<T> request);

private:
    static void *worker(void *arg); // pass to p_thread
    void run();

private:
    int m_thread_number;                  // max thread number
    int m_max_requests;                   // max requests
    std::unique_ptr<pthread_t> m_threads; // threads
    std::list<std::shared_ptr<T>> m_workqueue;
    locker m_queuelocker;
    sem m_queuestat; // have tasks?
    bool m_stop;     // terminate
};

template <typename T>
threadpool<T>::threadpool(int thread_number, int max_requests) : m_thread_number(thread_number), m_max_requests(max_requests), m_stop(false), m_threads(nullptr)
{
    if (thread_number <= 0 || max_requests <= 0)
    {
        throw std::exception();
    }
    m_threads = std::make_unique<pthread_t>(thread_number);
    if (!m_threads)
    {
        throw std::exception();
    }

    for (int i = 0; i < thread_number; ++i)
    {
        printf("create the %dth thread\n", i);
        if (pthread_create(m_threads + i, nullptr, worker, this) != 0)
        {
            throw std::exception();
        }

        if (pthread_detach(m_threads[i]))
        {
            throw std::exception();
        }
    }
}

template <typename T>
threadpool<T>::~threadpool()
{
    m_stop = true;
}

template <typename T>
bool threadpool<T>::append(std::shared_ptr<T> request)
{
    m_queuelocker.lock();
    if (m_workqueue.size() > m_max_requests)
    {
        m_queuelocker.unlock();
        return false;
    }
    m_workqueue.push_back(request);
    m_queuelocker.unlock();
    m_queuestat.post();
    return true;
}

template <typename T>
void *threadpool<T>::worker(void *arg)
{
    threadpool pool = (threadpool *)arg;
    pool->run();
    return pool;
}

template <typename T>
void threadpool<T>::run()
{
    while (!m_stop)
    {
        m_queuestat.wait();
        m_queuelocker.lock();
        if (m_workqueue.empty())
        {
            m_queuelocker.unlock();
            continue;
        }
        std::shared_ptr<T> request = m_workqueue.front();
        m_workqueue.pop_front();
        m_queuelocker.unlock();
        if (!request)
        {
            continue;
        }
        request->process();
    }
}
#endif