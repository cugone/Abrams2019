#include "Engine/Profiling/Memory.hpp"

#ifdef TRACK_MEMORY

    #if defined(_MSC_VER)
        #pragma warning(push)
        #pragma warning(disable : 28251) // Inconsistent annotation for 'new': this instance has no annotations.
        #pragma warning(disable : 28251) // Inconsistent annotation for 'new[]': this instance has no annotations.
    #endif

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

    #if defined(_MSC_VER)
        #pragma warning(pop)
    #endif

#endif