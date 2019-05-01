#include "Engine/Core/Stopwatch.hpp"

Stopwatch::Stopwatch(unsigned int frequency)
    : Stopwatch(TimeUtils::FPSeconds(1.0f / static_cast<float>(frequency)))
{
    /* DO NOTHING */
}

Stopwatch::Stopwatch(const TimeUtils::FPSeconds& seconds) {
    SetSeconds(seconds);
}

void Stopwatch::SetSeconds(const TimeUtils::FPSeconds& seconds) {
    interval_time = seconds;
    target_time = TimeUtils::FPSeconds{ TimeUtils::GetCurrentTimeElapsed()
        + seconds };
}

void Stopwatch::SetFrequency(unsigned int hz) {
    SetSeconds(TimeUtils::FPSeconds(1.0f / static_cast<float>(hz)));
}

bool Stopwatch::Check() {
    auto current_time = TimeUtils::GetCurrentTimeElapsed();
    return (target_time < current_time);
}

bool Stopwatch::CheckAndDecrement() {
    if(Check()) {
        target_time += interval_time;
        return true;
    } else {
        return false;
    }
}

bool Stopwatch::CheckAndReset() {
    if(Check()) {
        Reset();
        return true;
    } else {
        return false;
    }
}

unsigned int Stopwatch::DecrementAll() {
    unsigned int count = 0;
    while(CheckAndDecrement()) {
        ++count;
    }

    return count;
}

void Stopwatch::Reset() {
    target_time = TimeUtils::FPSeconds{ TimeUtils::GetCurrentTimeElapsed()
                             + interval_time };
}
