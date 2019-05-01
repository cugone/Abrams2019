#include "Engine/Profiling/Memory.hpp"

#ifdef TRACK_MEMORY

void* operator new(std::size_t size) {
    return Memory::allocate(size);
}

void* operator new[](std::size_t size) {
    return Memory::allocate(size);
}

void operator delete(void* ptr, std::size_t size) noexcept {
    Memory::deallocate(ptr, size);
}

void operator delete[](void* ptr, std::size_t size) noexcept {
    Memory::deallocate(ptr, size);
}

#endif