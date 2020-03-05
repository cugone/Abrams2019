#pragma once

#include <ostream>

namespace System::Ram {

struct RamDesc {
    unsigned long long installed{};
    unsigned long long available{};
    friend std::ostream& operator<<(std::ostream& out, const RamDesc& desc) noexcept;
};

RamDesc GetRamDesc() noexcept;

} // namespace System::Ram