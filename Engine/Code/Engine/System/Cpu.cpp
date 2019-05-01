#include "Engine/System/Cpu.hpp"

#include "Engine/Core/DataUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Win.hpp"

#include "Engine/System/OS.hpp"

#include <iomanip>
#include <memory>
#include <sstream>
#include <string>

System::Cpu::ProcessorArchitecture GetProcessorArchitecture();
unsigned long GetLogicalProcessorCount();
unsigned long GetSocketCount();
SYSTEM_INFO GetSystemInfo();

System::Cpu::CpuDesc System::Cpu::GetCpuDesc() {
    CpuDesc desc{};
    desc.type = GetProcessorArchitecture();
    desc.logicalCount = GetLogicalProcessorCount();
    desc.socketCount = GetSocketCount();
    return desc;
}

std::ostream& System::Cpu::operator<<(std::ostream& out, const System::Cpu::CpuDesc& cpu) {
    auto old_fmt = out.flags();
    auto old_w = out.width();
    out << std::left << std::setw(25) << "Processor Type:"           << std::right << std::setw(25) << StringUtils::to_string(cpu.type) << '\n';
    out << std::left << std::setw(25) << "Socket Count:"             << std::right << std::setw(25) << cpu.socketCount  << '\n';
    out << std::left << std::setw(25) << "Logical Processor Count:"  << std::right << std::setw(25) << cpu.logicalCount << '\n';
    out.flags(old_fmt);
    out.width(old_w);
    return out;
}

SYSTEM_INFO GetSystemInfo() {
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

System::Cpu::ProcessorArchitecture GetProcessorArchitecture() {
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

unsigned long GetLogicalProcessorCount() {
    SYSTEM_INFO info = GetSystemInfo();
    return info.dwNumberOfProcessors;
}

unsigned long GetSocketCount() {
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
