#include <heap_timer.h>

template <typename T>
void TimeHeap<T>::tick()
{
    time_t cur = time(nullptr);
    while (!heap.empty())
    {
        std::shared_ptr<HeapTimer<T>> cur_timer = heap.front();
        if (cur_timer->is_remove)
        {
            pop();
            continue;
        }
        if (cur_timer->expire > cur)
            break;
        cur_timer->call_back();
        pop();
    }
}

template <typename T>
void TimeHeap<T>::push(std::shared_ptr<T> client_data, time_t delay)
{
    HeapTimer<T> timer = HeapTimer<T>(client_data, delay);
    std::shared_ptr<HeapTimer<T>> timer_p = std::make_shared<HeapTimer<T>>(timer);

    heap.push_back(timer_p);
    std::push_heap(heap.begin(), heap.end());
}

template <typename T>
void TimeHeap<T>::remove(HeapTimer<T> timer)
{
    timer.is_remove = true;
}

template <typename T>
std::shared_ptr<HeapTimer<T>> TimeHeap<T>::front()
{
    return heap.front();
}

template <typename T>
int TimeHeap<T>::size()
{
    return heap.size();
}

template <typename T>
bool TimeHeap<T>::empty()
{
    return heap.empty();
}

template <typename T>
void TimeHeap<T>::pop()
{
    if (heap.empty())
        return;
    std::pop_heap(heap.begin(), heap.end());
    heap.pop_back();
}