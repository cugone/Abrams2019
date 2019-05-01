#pragma once

#include <cstddef>

template<typename T, std::size_t maxSize>
class MemoryPool {
public:
	MemoryPool();
    MemoryPool(const MemoryPool& other) = delete;
    MemoryPool(MemoryPool&& other) = delete;
    MemoryPool& operator=(const MemoryPool& rhs) = delete;
    MemoryPool& operator=(MemoryPool&& rhs) = delete;
	~MemoryPool();

    [[nodiscard]] void* allocate(std::size_t size);
    void deallocate(void* ptr, std::size_t size);

protected:
private:

    std::size_t _count = 0;
    std::size_t _max = 0;
    T* _data = nullptr;
    T* _ptr = nullptr;
};

template<typename T, std::size_t maxSize>
[[nodiscard]] void* MemoryPool<T, maxSize>::allocate(std::size_t size) {
    std::size_t elems = size / sizeof(T);
    if(_count + elems < _max) {
        auto front = _ptr;
        _count += elems;
        _ptr += _count;
        return reinterpret_cast<void*>(front);
    }
    return nullptr;
}

template<typename T, std::size_t maxSize>
void MemoryPool<T, maxSize>::deallocate(void* ptr, std::size_t size) {
    auto elems = static_cast<int>(size / sizeof(T));
    if(0 < _count - elems) {
        _ptr -= elems;
        _count -= elems;
    } else {
        _ptr = _data;
        _count = 0;
    }
}

template<typename T, std::size_t maxSize>
MemoryPool<T, maxSize>::~MemoryPool() {
    std::free(_data);
    _data = nullptr;
    _ptr = nullptr;
    _count = 0;
    _max = 0;
}

template<typename T, std::size_t maxSize>
MemoryPool<T, maxSize>::MemoryPool() {
    _data = reinterpret_cast<T*>(std::malloc(maxSize * sizeof(T)));
    _ptr = _data;
    _count = 0;
    _max = maxSize;
}
