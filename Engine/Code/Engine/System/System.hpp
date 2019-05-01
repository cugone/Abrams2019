#pragma once

#include "Engine/System/Cpu.hpp"
#include "Engine/System/Ram.hpp"
#include "Engine/System/OS.hpp"

#include <ostream>

namespace System {

struct SystemDesc {
    System::Ram::RamDesc ram{};
    System::Cpu::CpuDesc cpu{};
    System::OS::OsDesc os{};
    friend std::ostream& operator<<(std::ostream& out, const SystemDesc& desc);
};

SystemDesc GetSystemDesc();

}