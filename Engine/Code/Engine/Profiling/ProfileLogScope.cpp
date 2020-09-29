#include "Engine/Profiling/ProfileLogScope.hpp"

#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/TimeUtils.hpp"

#include <iomanip>
#include <sstream>

ProfileLogScope::ProfileLogScope(const char* scopeName) noexcept
: _scope_name(scopeName)
, _time_at_creation(TimeUtils::Now()) {
    /* DO NOTHING */
}

ProfileLogScope::~ProfileLogScope() noexcept {
    const auto now = TimeUtils::Now();
    TimeUtils::FPMicroseconds elapsedTime = (now - _time_at_creation);
    DebuggerPrintf("ProfileLogScope %s took %.02f us.\n", _scope_name, elapsedTime.count());
}
