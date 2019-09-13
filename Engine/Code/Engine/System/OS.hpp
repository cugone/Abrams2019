#pragma once

#include <string>
#include <ostream>

namespace System::OS {

    enum class OperatingSystem : uint32_t {
        Unknown             = 0b0000'0000'0000'0000'0000'0000'0000'0000
        , Windows           = 0b0000'0000'0000'0000'0000'0000'0000'0001
        , Windows_x86       = 0b0000'0000'0000'0000'0000'0000'0000'0010
        , Windows_x64       = 0b0000'0000'0000'0000'0000'0000'0000'0100
        , Windows_Xp        = 0b0000'0000'0000'0000'0000'0000'0000'1000
        , Windows_XpSp1     = 0b0000'0000'0000'0000'0000'0000'0001'0000
        , Windows_XpSp2     = 0b0000'0000'0000'0000'0000'0000'0010'0000
        , Windows_XpSp3     = 0b0000'0000'0000'0000'0000'0000'0100'0000
        , Windows_Vista     = 0b0000'0000'0000'0000'0000'0000'1000'0000
        , Windows_VistaSp1  = 0b0000'0000'0000'0000'0000'0001'0000'0000
        , Windows_VistaSp2  = 0b0000'0000'0000'0000'0000'0010'0000'0000
        , Windows_7         = 0b0000'0000'0000'0000'0000'0100'0000'0000
        , Windows_7Sp1      = 0b0000'0000'0000'0000'0000'1000'0000'0000
        , Windows_8         = 0b0000'0000'0000'0000'0001'0000'0000'0000
        , Windows_8Point1   = 0b0000'0000'0000'0000'0010'0000'0000'0000
        , Windows_10        = 0b0000'0000'0000'0000'0100'0000'0000'0000
        , Windows_Server    = 0b0000'0000'0000'0000'1000'0000'0000'0000
    };

    enum class OperatingSystemArchitecture : uint8_t {
        Unknown
        , x86
        , x64
    };

    OperatingSystem& operator&=(OperatingSystem& a, const OperatingSystem& b) noexcept;
    OperatingSystem& operator|=(OperatingSystem& a, const OperatingSystem& b) noexcept;
    OperatingSystem  operator&(OperatingSystem a, const OperatingSystem& b) noexcept;
    OperatingSystem  operator|(OperatingSystem a, const OperatingSystem& b) noexcept;
    OperatingSystem  operator~(const OperatingSystem& a) noexcept;

    struct OsDesc {
        OperatingSystem type{};
        OperatingSystemArchitecture architecture{};
        std::string VersionFriendly{};
        friend std::ostream& operator<<(std::ostream& out, const OsDesc& cpu) noexcept;
    };

    OsDesc GetOsDesc() noexcept;

    OperatingSystem GetOperatingSystemType() noexcept;
    OperatingSystemArchitecture GetOperatingSystemArchitecture() noexcept;
    std::string GetFriendlyStringFromOperatingSystemType(System::OS::OperatingSystem type) noexcept;

}