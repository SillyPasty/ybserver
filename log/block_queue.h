#ifndef BLOCK_QUEUE_H
#define BLOCK_QUEUE_H
#include <queue>
#include <vector>
#include <sys/time.h>
#include "../locker/locker.h"

template <typename T>
class BlockQueue {
public:
    BlockQueue(int max_size = 1000);
    ~BlockQueue() = default;
    T pop(int timeout_ms = 100);
    bool push(const T value);
    int size();
    int max_size();
private:
    int bq_max_size;
    int bq_front;
    int bq_end;
    locker m_mutex;
    cond m_cond;
    std::deque<T> bq;
};

template<typename T>
BlockQueue<T>::BlockQueue(int max_size):
    bq_max_size(max_size), bq_front(0), bq_end(0) {
        bq = std::deque<T>();
    }

template <typename T>
T BlockQueue<T>::pop(int ms_timeout) {
    struct timespec t = {0, 0};
    struct timeval now = {0, 0};
    gettimeofday(&now, NULL);
    m_mutex.lock();
    if (bq.empty()) {
        t.tv_sec = now.tv_sec + ms_timeout / 1000;
        t.tv_nsec = (ms_timeout % 1000) * 1000;
        if (!m_cond.timewait(m_mutex.get(), t)) {
            m_mutex.unlock();
            return T();
        }
    }
    T front = bq.front();
    bq.pop_front();
    m_mutex.unlock();
    return front;
}

template <typename T>
bool BlockQueue<T>::push(T value) {
    m_mutex.lock();
    if (bq.size() >= bq_max_size) {
        m_mutex.unlock();
        return false;
    }
    bq.push_back(value);
    m_cond.signal();
    m_mutex.unlock();
    return true;
}

template <typename T>
int BlockQueue<T>::max_size() {
    return bq_max_size;
}

template <typename T>
int BlockQueue<T>::size() {
    m_mutex.lock();
    int ret = bq.size();
    m_mutex.unlock();
    return ret;
}
#endif