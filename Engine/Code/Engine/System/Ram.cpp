#include "Engine/System/Ram.hpp"

#include "Engine/Math/MathUtils.hpp"

#include "Engine/Core/Win.hpp"

#include <iomanip>

unsigned long long GetPhysicalRam() noexcept;
unsigned long long GetAvailableRam() noexcept;

std::ostream& System::Ram::operator<<(std::ostream& out, const System::Ram::RamDesc& desc) noexcept {
    auto old_fmt = out.flags();
    auto old_w = out.width();
    out << std::left << std::setw(22) << "Installed RAM:" << std::right << std::setw(30) << std::fixed << std::setprecision(1) << desc.installed * MathUtils::GIB_BYTES_RATIO << " GB\n";
    out << std::left << std::setw(22) << "Available RAM:" << std::right << std::setw(30) << std::fixed << std::setprecision(1) << desc.available * MathUtils::GIB_BYTES_RATIO << " GB\n";
    out.flags(old_fmt);
    out.width(old_w);
    return out;
}

System::Ram::RamDesc System::Ram::GetRamDesc() noexcept {
    RamDesc desc{};
    desc.available = GetAvailableRam();
    desc.installed = GetPhysicalRam();
    return desc;
}

unsigned long long GetPhysicalRam() noexcept {
    uint64_t pram = 0;
    ::GetPhysicallyInstalledSystemMemory(&pram);
    return static_cast<unsigned long long>(pram * MathUtils::BYTES_KIB_RATIO);
}

unsigned long long GetAvailableRam() noexcept {
    MEMORYSTATUSEX mem{};
    mem.dwLength = sizeof(MEMORYSTATUSEX);
    ::GlobalMemoryStatusEx(&mem);
    return mem.ullTotalPhys;
}

