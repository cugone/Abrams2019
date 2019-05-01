#pragma once
//https://www.youtube.com/watch?v=e2ZQyYr0Oi0
//C++17 - The Best Features - Nicolai Josuttis [ACCU 2018]

#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Profiling/StackTrace.hpp"

#include <new>
#include <sstream>

class Memory {
public:

    struct status_t {
        std::size_t leaked_objs  = 0;
        std::size_t leaked_bytes = 0;
        operator bool() {
            return leaked_objs || leaked_bytes;
        }
        operator std::string() {
#ifdef TRACK_MEMORY
            std::ostringstream ss;
            std::string s = ss.str();
            ss << "Leaked objects: " << leaked_objs << " for " << leaked_bytes << " bytes.\n";
            return s;
#else
            return {};
#endif
        }
        friend std::ostream& operator<<(std::ostream& os, const status_t s) {
#ifdef TRACK_MEMORY
            os << "Leaked objects: " << s.leaked_objs << " for " << s.leaked_bytes << " bytes.\n";
#endif
            return os;
        }
    };
    struct status_frame_t {
        std::size_t frame_id = 0;
        std::size_t leaked_objs = 0;
        std::size_t leaked_bytes = 0;
        operator bool() {
            return leaked_objs || leaked_bytes;
        }
        operator std::string() {
#ifdef TRACK_MEMORY
            std::ostringstream ss;
            ss << "Frame " << frame_id << ": Leaked objects: " << leaked_objs << " for " << leaked_bytes << " bytes.\n";
            std::string s = ss.str();
            return s;
#else
            return {};
#endif
        }
    };

    [[nodiscard]] static void* allocate(std::size_t n) {
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

    static void enable([[maybe_unused]]bool e) {
#ifdef TRACK_MEMORY
        _active = e;
#endif
    }

    static bool is_enabled() {
#ifdef TRACK_MEMORY
        return _active;
#else
        return false;
#endif
    }

    static void trace([[maybe_unused]]bool doTrace) {
#ifdef TRACK_MEMORY
        _trace = doTrace;
#endif
    }

    static void tick() {
#ifdef TRACK_MEMORY
        if(auto f = Memory::frame_status()) {
            std::string status = f;
            DebuggerPrintf(status.c_str(), "%s");
        }
        ++frameCounter;
        resetframecounters();
#endif
    }

    static void resetframecounters() {
#ifdef TRACK_MEMORY
        frameSize = 0;
        frameCount = 0;
        framefreeCount = 0;
        framefreeSize = 0;
#endif
    }

    static status_t status() {
        return { allocCount - freeCount, allocSize - freeSize };
    }

    static status_frame_t frame_status() {
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
    inline static bool _active = false;
    inline static bool _trace = false;
};

#ifdef TRACK_MEMORY

void* operator new(std::size_t size);
void* operator new[](std::size_t size);
void operator delete(void* ptr, std::size_t size) noexcept;
void operator delete[](void* ptr, std::size_t size) noexcept;

#endif
