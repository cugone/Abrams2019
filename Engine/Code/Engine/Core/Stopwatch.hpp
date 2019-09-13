#pragma once

#include "Engine/Core/TimeUtils.hpp"

#include <chrono>
#include <ratio>

class Stopwatch {
public:
    Stopwatch() = default;
    Stopwatch(const Stopwatch& other) = default;
    Stopwatch(Stopwatch&& r_other) = default;
    Stopwatch& operator=(const Stopwatch& rhs) = default;
    Stopwatch& operator=(Stopwatch&& rhs) = default;
    explicit Stopwatch(const TimeUtils::FPSeconds& seconds) noexcept;
    explicit Stopwatch(unsigned int frequency) noexcept;
    ~Stopwatch() = default;

    void SetSeconds(const TimeUtils::FPSeconds& seconds) noexcept;
    void SetFrequency(unsigned int hz) noexcept;
    bool Check() const noexcept;
    bool CheckAndDecrement() noexcept;
    bool CheckAndReset() noexcept;
    unsigned int DecrementAll() noexcept;
    void Reset() noexcept;
private:
    TimeUtils::FPSeconds interval_time{};
    TimeUtils::FPSeconds target_time{};

};