#include "Engine/Profiling/ProfileLogScope.hpp"

#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/TimeUtils.hpp"

#include <iomanip>
#include <sstream>

ProfileLogScope::ProfileLogScope(const std::string& scopeName) noexcept
    : _scope_name(scopeName)
    , _time_at_creation(TimeUtils::Now())
{
    /* DO NOTHING */
}

ProfileLogScope::~ProfileLogScope() noexcept {
    auto now = TimeUtils::Now();
    TimeUtils::FPMicroseconds elapsedTime = (now - _time_at_creation);
    std::ostringstream ss;
    ss << "ProfileLogScope " << _scope_name << " took " << elapsedTime.count() << " us.\n";
    DebuggerPrintf(ss.str().c_str());
    //DebuggerPrintf("ProfileLogScope %s took %.02f us.\n", _scope_name.c_str(), elapsedTime.count());
}

