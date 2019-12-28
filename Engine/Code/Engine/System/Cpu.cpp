#include "Engine/System/Cpu.hpp"

#include "Engine/Core/DataUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Win.hpp"

#include "Engine/System/OS.hpp"

#include <iomanip>
#include <memory>
#include <sstream>
#include <string>


std::string StringUtils::to_string(const System::Cpu::ProcessorArchitecture& architecture) noexcept {
    using namespace System::Cpu;
    switch(architecture) {
    case ProcessorArchitecture::Unknown:
        return "Unknown";
    case ProcessorArchitecture::x64: //Also Amd64
        return "x64";
    case ProcessorArchitecture::Arm:
        return "ARM";
    case ProcessorArchitecture::Arm64:
        return "ARM 64";
    case ProcessorArchitecture::Ia64:
        return "Intel Itanium 64";
    case ProcessorArchitecture::x86: //Also Intel
        return "x86";
    case ProcessorArchitecture::Mips:
        return "Mips";
    case ProcessorArchitecture::Alpha:
        return "Alpha";
    case ProcessorArchitecture::Ppc:
        return "PPC";
    case ProcessorArchitecture::Shx:
        return "SHX";
    case ProcessorArchitecture::Alpha64:
        return "Alpha 64";
    case ProcessorArchitecture::Msil:
        return "MSIL";
    case ProcessorArchitecture::Ia32OnWin64:
        return "Intel Itanium on Win64";
    case ProcessorArchitecture::Neutral:
        return "Neutral";
    case ProcessorArchitecture::Arm32OnWin64:
        return "ARM32 on Win64";
    case ProcessorArchitecture::Ia32OnArm64:
        return "Intel Itanium on ARM64";
    default:
        return "";
    }
}

System::Cpu::ProcessorArchitecture GetProcessorArchitecture() noexcept;
unsigned long GetLogicalProcessorCount() noexcept;
unsigned long GetSocketCount() noexcept;
SYSTEM_INFO GetSystemInfo() noexcept;

System::Cpu::CpuDesc System::Cpu::GetCpuDesc() noexcept {
    CpuDesc desc{};
    desc.type = GetProcessorArchitecture();
    desc.logicalCount = GetLogicalProcessorCount();
    desc.socketCount = GetSocketCount();
    return desc;
}

std::ostream& System::Cpu::operator<<(std::ostream& out, const System::Cpu::CpuDesc& cpu) noexcept {
    auto old_fmt = out.flags();
    auto old_w = out.width();
    out << std::left << std::setw(25) << "Processor Type:"           << std::right << std::setw(27) << StringUtils::to_string(cpu.type) << '\n';
    out << std::left << std::setw(25) << "Socket Count:"             << std::right << std::setw(27) << cpu.socketCount  << '\n';
    out << std::left << std::setw(25) << "Logical Processor Count:"  << std::right << std::setw(27) << cpu.logicalCount << '\n';
    out.flags(old_fmt);
    out.width(old_w);
    return out;
}

SYSTEM_INFO GetSystemInfo() noexcept {
    SYSTEM_INFO info{};
    switch(System::OS::GetOperatingSystemArchitecture()) {
    case System::OS::OperatingSystemArchitecture::x86:
    {
        ::GetSystemInfo(&info);
        return info;
    }
    case System::OS::OperatingSystemArchitecture::x64:
    {
        ::GetNativeSystemInfo(&info);
        return info;
    }
    case System::OS::OperatingSystemArchitecture::Unknown:
    default:
    {
        return info;
    }
    }
}

System::Cpu::ProcessorArchitecture GetProcessorArchitecture() noexcept {
    using namespace System::Cpu;
    auto info = GetSystemInfo();
    switch(info.wProcessorArchitecture) {
    case PROCESSOR_ARCHITECTURE_INTEL: return ProcessorArchitecture::Intel;
    case PROCESSOR_ARCHITECTURE_MIPS: return ProcessorArchitecture::Mips;
    case PROCESSOR_ARCHITECTURE_ALPHA: return ProcessorArchitecture::Alpha;
    case PROCESSOR_ARCHITECTURE_PPC: return ProcessorArchitecture::Ppc;
    case PROCESSOR_ARCHITECTURE_SHX: return ProcessorArchitecture::Shx;
    case PROCESSOR_ARCHITECTURE_ARM: return ProcessorArchitecture::Arm;
    case PROCESSOR_ARCHITECTURE_IA64: return ProcessorArchitecture::Ia64;
    case PROCESSOR_ARCHITECTURE_ALPHA64: return ProcessorArchitecture::Alpha64;
    case PROCESSOR_ARCHITECTURE_MSIL: return ProcessorArchitecture::Msil;
    case PROCESSOR_ARCHITECTURE_AMD64: return ProcessorArchitecture::Amd64;
    case PROCESSOR_ARCHITECTURE_IA32_ON_WIN64: return ProcessorArchitecture::Ia32OnWin64;
    case PROCESSOR_ARCHITECTURE_NEUTRAL: return ProcessorArchitecture::Neutral;
    case PROCESSOR_ARCHITECTURE_ARM64: return ProcessorArchitecture::Arm64;
    case PROCESSOR_ARCHITECTURE_ARM32_ON_WIN64: return ProcessorArchitecture::Arm32OnWin64;
    case PROCESSOR_ARCHITECTURE_IA32_ON_ARM64: return ProcessorArchitecture::Ia32OnArm64;
    case PROCESSOR_ARCHITECTURE_UNKNOWN: return ProcessorArchitecture::Unknown;
    default: return ProcessorArchitecture::Unknown;
    }
}

unsigned long GetLogicalProcessorCount() noexcept {
    SYSTEM_INFO info = GetSystemInfo();
    return info.dwNumberOfProcessors;
}

unsigned long GetSocketCount() noexcept {
    DWORD length{};
    unsigned long socketCount{};
    //This will intentionally fail in order to fill the length parameter with the correct value.
    if(!::GetLogicalProcessorInformation(nullptr, &length)) {
        if(::GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
            auto b = std::make_unique<unsigned char[]>(length);
            auto buffer = reinterpret_cast<SYSTEM_LOGICAL_PROCESSOR_INFORMATION*>(b.get());
            if(!::GetLogicalProcessorInformation(buffer, &length)) {
                return 0ul;
            }
            std::stringstream ss(std::ios_base::binary | std::ios_base::in | std::ios_base::out);
            if(!ss.write(reinterpret_cast<const char*>(buffer), length)) {
                return 0ul;
            }
            ss.clear();
            ss.seekg(0);
            ss.seekp(0);
            SYSTEM_LOGICAL_PROCESSOR_INFORMATION p{};
            while(ss.read(reinterpret_cast<char*>(&p), sizeof(p))) {
                switch(p.Relationship) {
                case RelationProcessorPackage:
                {
                    ++socketCount;
                    break;
                }
                default:
                    break;
                }
            }
        }
    }
    return socketCount;
}
