#pragma once

#include <mutex>
#include <queue>

template<typename T>
class ThreadSafeQueue {
public:
    void push(const T& t) noexcept;
    void pop() noexcept;

    template<class... Args>
    decltype(auto) emplace(Args&&... args) {
        return _queue.emplace(std::forward<Args>(args)...);
    }


    [[nodiscard]] decltype(auto) size() const noexcept;
    [[nodiscard]] bool empty() const noexcept;

    [[nodiscard]] T& back() const noexcept;
    [[nodiscard]] T& back() noexcept;
    [[nodiscard]] T& front() const noexcept;
    [[nodiscard]] T& front() noexcept;

    void swap(ThreadSafeQueue<T>& b) noexcept;

protected:
private:
    mutable std::mutex _cs{};
    std::queue<T> _queue{};
};

template<typename T>
void ThreadSafeQueue<T>::swap(ThreadSafeQueue<T>& b) noexcept {
    std::scoped_lock<std::mutex, std::mutex> lock(_cs, b._cs);
    _queue.swap(b._queue);
}

template<typename T>
void ThreadSafeQueue<T>::push(const T& t) noexcept {
    std::scoped_lock<std::mutex> lock(_cs);
    _queue.push(t);
}

template<typename T>
void ThreadSafeQueue<T>::pop() noexcept {
    std::scoped_lock<std::mutex> lock(_cs);
    _queue.pop();
}

template<typename T>
decltype(auto) ThreadSafeQueue<T>::size() const noexcept {
    std::scoped_lock<std::mutex> lock(_cs);
    return _queue.size();
}

template<typename T>
bool ThreadSafeQueue<T>::empty() const noexcept {
    std::scoped_lock<std::mutex> lock(_cs);
    return _queue.empty();
}

template<typename T>
T& ThreadSafeQueue<T>::back() const noexcept {
    std::scoped_lock<std::mutex> lock(_cs);
    return _queue.back();
}

template<typename T>
T& ThreadSafeQueue<T>::back() noexcept {
    std::scoped_lock<std::mutex> lock(_cs);
    return _queue.back();
}

template<typename T>
T& ThreadSafeQueue<T>::front() const noexcept {
    std::scoped_lock<std::mutex> lock(_cs);
    return _queue.front();
}

template<typename T>
T& ThreadSafeQueue<T>::front() noexcept {
    std::scoped_lock<std::mutex> lock(_cs);
    return _queue.front();
}
