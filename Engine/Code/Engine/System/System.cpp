#include "Engine/System/System.hpp"

std::ostream& System::operator<<(std::ostream& out, const System::SystemDesc& desc) noexcept {
    auto old_fmt = out.flags();
    auto old_w = out.width();
    out << "SYSTEM:\n";
    out << desc.os;
    out << desc.cpu;
    out << desc.ram;
    out << '\n';
    out.flags(old_fmt);
    out.width(old_w);
    return out;
}

System::SystemDesc System::GetSystemDesc() noexcept {
    SystemDesc desc{};
    desc.cpu = Cpu::GetCpuDesc();
    desc.ram = Ram::GetRamDesc();
    desc.os = OS::GetOsDesc();
    return desc;
}
