#include "Engine/Core/TimeUtils.hpp"

#include "Engine/Core/Win.hpp"

#include <ctime>
#include <sstream>
#include <iomanip>

namespace TimeUtils {

std::string GetDateTimeStampFromNow(const DateTimeStampOptions& options /*= DateTimeStampOptions{}*/) noexcept {
    using namespace std::chrono;
    auto now = Now<system_clock>();
    std::time_t t = system_clock::to_time_t(now);
    std::tm tm;
    ::localtime_s(&tm, &t);
    std::ostringstream msg;
    std::string fmt = options.use_24_hour_clock ? (options.use_separator ? (options.is_filename ? "%Y-%m-%d_%H%M%S" : "%Y-%m-%d %H:%M:%S") : "%Y%m%d%H%M%S")
                                                : (options.use_separator ? (options.is_filename ? "%Y-%m-%d_%I%M%S" : "%Y-%m-%d %I:%M:%S") : "%Y%m%d%I%M%S");
    msg << std::put_time(&tm, fmt.c_str());

    if(options.include_milliseconds) {
        auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1s;
        if(options.use_separator) {
            msg << (options.is_filename ? "_" : ".");
        }
        msg << std::fixed << std::right << std::setw(3) << std::setfill('0') << ms.count();
    }

    return msg.str();
}

std::string GetTimeStampFromNow(const DateTimeStampOptions& options /*= DateTimeStampOptions{}*/) noexcept {
    using namespace std::chrono;
    auto now = Now<system_clock>();
    auto t = system_clock::to_time_t(now);
    std::tm tm;
    ::localtime_s(&tm, &t);
    std::ostringstream msg;
    std::string fmt = options.use_24_hour_clock ? (options.use_separator ? (options.is_filename ? "%H-%M-%S" : "%H:%M:%S") : "%H%M%S")
                                                : (options.use_separator ? (options.is_filename ? "%I-%M-%S" : "%I:%M:%S") : "%I%M%S");
    msg << std::put_time(&tm, fmt.c_str());

    if(options.include_milliseconds) {
        auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1s;
        if(options.use_separator) {
            msg << (options.is_filename ? '_' : '.');
        }
        msg << std::fixed << std::setw(3) << std::setfill('0') << ms.count();
    }

    return msg.str();
}

std::string GetDateStampFromNow(const DateTimeStampOptions& options /*= DateTimeStampOptions{}*/) noexcept {
    using namespace std::chrono;
    auto now = Now<system_clock>();
    auto t = system_clock::to_time_t(now);
    std::tm tm;
    ::localtime_s(&tm, &t);
    std::stringstream msg;
    std::string fmt = options.use_separator ? "%Y-%m-%d" : "%Y%m%d";
    msg << std::put_time(&tm, fmt.c_str());
    return msg.str();
}

}
