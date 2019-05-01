#pragma once

#include <queue>
#include <mutex>

template<typename T>
class ThreadSafeQueue {
public:
    ThreadSafeQueue() = default;
    ~ThreadSafeQueue();

    void push(const T& t);
    void pop();
    decltype(auto) size() const;
    bool empty() const;

    T& back() const;
    T& back();
    T& front() const;
    T& front();

protected:
private:
    mutable std::mutex _cs{};
    std::queue<T> _queue{};
};

template<typename T>
ThreadSafeQueue<T>::~ThreadSafeQueue() {
    std::scoped_lock<std::mutex> lock(_cs);
    std::queue<T> temp{};
    _queue.swap(temp);
}

template<typename T>
void ThreadSafeQueue<T>::push(const T& t) {
    std::scoped_lock<std::mutex> lock(_cs);
    _queue.push(t);
}

template<typename T>
void ThreadSafeQueue<T>::pop() {
    std::scoped_lock<std::mutex> lock(_cs);
    _queue.pop();
}

template<typename T>
decltype(auto) ThreadSafeQueue<T>::size() const {
    std::scoped_lock<std::mutex> lock(_cs);
    return _queue.size();
}

template<typename T>
bool ThreadSafeQueue<T>::empty() const {
    std::scoped_lock<std::mutex> lock(_cs);
    return _queue.empty();
}

template<typename T>
T& ThreadSafeQueue<T>::back() const {
    std::scoped_lock<std::mutex> lock(_cs);
    return _queue.back();
}

template<typename T>
T& ThreadSafeQueue<T>::back() {
    std::scoped_lock<std::mutex> lock(_cs);
    return _queue.back();
}

template<typename T>
T& ThreadSafeQueue<T>::front() const {
    std::scoped_lock<std::mutex> lock(_cs);
    return _queue.front();
}

template<typename T>
T& ThreadSafeQueue<T>::front() {
    std::scoped_lock<std::mutex> lock(_cs);
    return _queue.front();
}
