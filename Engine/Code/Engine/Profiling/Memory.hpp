#pragma once
//https://www.youtube.com/watch?v=e2ZQyYr0Oi0
//C++17 - The Best Features - Nicolai Josuttis [ACCU 2018]

#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#include <array>
#include <charconv>
#include <cstring>
#include <iostream>
#include <new>
#include <ostream>
#include <string_view>
#include <string>
#include <sstream>

class Memory {
public:

    struct status_t {
        std::size_t leaked_objs  = 0;
        std::size_t leaked_bytes = 0;
        operator bool() const noexcept {
            return leaked_objs || leaked_bytes;
        }
        friend std::ostream& operator<<(std::ostream& os, [[maybe_unused]]const status_t& s) noexcept {
#ifdef TRACK_MEMORY
            static std::array<char, 1024> str{ "Leaked objects: %f for %f bytes." };
            std::to_chars_result first_result;
            if (first_result = std::to_chars(str.data() + 16, str.data() + 20, static_cast<float>(s.leaked_objs));
                first_result.ec != std::errc()) {
                DebuggerPrintf("Error code %d: Memory profiler could not convert leaked objects value for printing", first_result.ec);
                return os;
            }
            //Put remainder of string back into buffer because to_chars overwrites.
            std::memcpy(first_result.ptr, " for %f bytes.", 16);
            std::to_chars_result second_result;
            if (second_result = std::to_chars(first_result.ptr + 5, first_result.ptr + 11, static_cast<float>(s.leaked_bytes));
                second_result.ec != std::errc()) {
                DebuggerPrintf("Error code %d: Memory profiler could not convert leaked bytes value for printing", second_result.ec);
                return os;
            }
            std::memcpy(second_result.ptr, " bytes.", 9);
            std::size_t length = 0;
            auto c = str.data();
            while(*c++) {
                length++;
            }
            os << std::string_view(str.data(), length);
#endif
            return os;
        }
    };
    struct status_frame_t {
        std::size_t frame_id = 0;
        std::size_t leaked_objs = 0;
        std::size_t leaked_bytes = 0;
        operator bool() const noexcept {
            return leaked_objs || leaked_bytes;
        }
        friend std::ostream& operator<<(std::ostream& os, [[maybe_unused]]const status_frame_t& s) noexcept {
#ifdef TRACK_MEMORY
            static std::array<char, 1024> str{ "Frame %f: Leaked objects: %f for %f bytes." };
            std::to_chars_result frame_result;
            if (frame_result = std::to_chars(str.data() + 6, str.data() + 26, static_cast<float>(s.frame_id));
                frame_result.ec != std::errc()) {
                DebuggerPrintf("Error code %d: Memory profiler could not convert frame id value for printing", frame_result.ec);
                return os;
            }
            //Put remainder of string back into buffer because to_chars overwrites.
            std::memcpy(frame_result.ptr, ": Leaked objects: %f for %f bytes.", 36);
            std::to_chars_result objects_result;
            if (objects_result = std::to_chars(frame_result.ptr + 18, frame_result.ptr + 38, static_cast<float>(s.leaked_objs));
                objects_result.ec != std::errc()) {
                DebuggerPrintf("Error code %d: Memory profiler could not convert leaked objects value for printing", objects_result.ec);
                return os;
            }
            std::memcpy(objects_result.ptr, " for %f bytes.", 16);
            std::to_chars_result bytes_result;
            if (bytes_result = std::to_chars(objects_result.ptr + 5, objects_result.ptr + 25, static_cast<float>(s.leaked_bytes));
                bytes_result.ec != std::errc()) {
                DebuggerPrintf("Error code %d: Memory profiler could not convert leaked bytes value for printing", bytes_result.ec);
                return os;
            }
            std::memcpy(bytes_result.ptr, " bytes.", 9);
            std::size_t length = 0;
            auto c = str.data();
            while (*c++) {
                ++length;
            }
            os << std::string_view(str.data(), length);
#endif
            return os;
        }
    };

    [[nodiscard]] static void* allocate(std::size_t n) noexcept {
        if(is_enabled()) {
            ++frameCount;
            frameSize += n;
            ++allocCount;
            allocSize += n;
            if(maxSize < allocSize) {
                maxSize = allocSize;
            }
            if(maxCount < allocCount) {
                maxCount = allocCount;
            }
        }
        return std::malloc(n);
    }

    static void deallocate(void* ptr, std::size_t size) noexcept {
        if(is_enabled()) {
            ++framefreeCount;
            framefreeSize += size;
            ++freeCount;
            freeSize += size;
        }
        std::free(ptr);
    }

    static void enable([[maybe_unused]]bool e) noexcept {
#ifdef TRACK_MEMORY
        _active = e;
        if(_active) {
            resetallcounters();
        }
#endif
    }

    static bool is_enabled() noexcept {
#ifdef TRACK_MEMORY
        return _active;
#else
        return false;
#endif
    }

    static void trace([[maybe_unused]]bool doTrace) noexcept {
#ifdef TRACK_MEMORY
        _trace = doTrace;
#endif
    }

    static void tick() noexcept {
#ifdef TRACK_MEMORY
        if(auto f = Memory::frame_status()) {
            std::cout << f << '\n';
        }
        ++frameCounter;
        resetframecounters();
#endif
    }

    static void resetframecounters() noexcept {
#ifdef TRACK_MEMORY
        frameSize = 0;
        frameCount = 0;
        framefreeCount = 0;
        framefreeSize = 0;
#endif
    }

    static void resetstatuscounters() noexcept {
#ifdef TRACK_MEMORY
        maxSize = 0;
        maxCount = 0;
        allocSize = 0;
        allocCount = 0;
        freeCount = 0;
        freeSize = 0;
#endif
    }

    static void resetallcounters() noexcept {
#ifdef TRACK_MEMORY
        resetframecounters();
        resetstatuscounters();
#endif
    }

    static status_t status() noexcept {
        return { allocCount - freeCount, allocSize - freeSize };
    }

    static status_frame_t frame_status() noexcept {
        return { frameCounter, frameCount - framefreeCount, frameSize - framefreeSize };
    }

    inline static std::size_t maxSize = 0;
    inline static std::size_t maxCount = 0;
    inline static std::size_t allocSize = 0;
    inline static std::size_t allocCount = 0;
    inline static std::size_t frameSize = 0;
    inline static std::size_t frameCount = 0;
    inline static std::size_t frameCounter = 0;
    inline static std::size_t freeCount = 0;
    inline static std::size_t freeSize = 0;
    inline static std::size_t framefreeCount = 0;
    inline static std::size_t framefreeSize = 0;
protected:
private:
    inline static bool _active = true;
    inline static bool _trace = false;
};

#ifdef TRACK_MEMORY

void* operator new(std::size_t size);
void* operator new[](std::size_t size);
void operator delete(void* ptr, std::size_t size) noexcept;
void operator delete[](void* ptr, std::size_t size) noexcept;

#endif
