#ifndef HEAP_TIMER_H
#define HEAP_TIMER_H
#include <vector>
#include <algorithm>
#include <memory>
#include <ctime>
template <typename T>
class HeapTimer
{
public:
    HeapTimer(std::shared_ptr<T> client_data, time_t delay)
        : data(client_data), is_remove(false)
    {
        exipre = time(nullptr) + delay;
    }
    ~HeapTimer() = default;
    void call_back()
    {
        return data->call_back();
    }

public:
    time_t expire;
    std::shared_ptr<T> data;
    bool is_remove;
};

template <typename T>
class TimeHeap
{
public:
    TimeHeap() = default;
    ~TimeHeap() = default;
    void tick();
    void push(std::shared_ptr<T>, time_t);
    std::shared_ptr<HeapTimer<T>> front();
    void remove(HeapTimer<T>);
    bool empty();
    int size();

private:
    void pop();
    std::vector<std::shared_ptr<HeapTimer<T>>> heap;
};

#endif