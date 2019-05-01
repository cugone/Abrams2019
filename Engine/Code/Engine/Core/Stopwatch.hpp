#pragma once

#include "Engine/Core/TimeUtils.hpp"

#include <chrono>
#include <ratio>

class Stopwatch {
public:
    explicit Stopwatch(const TimeUtils::FPSeconds& seconds);
    explicit Stopwatch(unsigned int frequency);
    void SetSeconds(const TimeUtils::FPSeconds& seconds);
    void SetFrequency(unsigned int hz);
    bool Check();
    bool CheckAndDecrement();
    bool CheckAndReset();
    unsigned int DecrementAll();
    void Reset();
private:
    TimeUtils::FPSeconds interval_time{};
    TimeUtils::FPSeconds target_time{};

};