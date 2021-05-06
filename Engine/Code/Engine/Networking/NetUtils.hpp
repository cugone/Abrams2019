#pragma once

#include "Engine/Networking/Address.hpp"

namespace a2de {
    namespace Net {
        //TODO: Implement Networking subsystem
        bool Initialize() noexcept;
        Address GetIpv4(Address& addy) noexcept;
        Address GetIpv6(Address& addy) noexcept;
        void Shutdown() noexcept;

    } // namespace Net
} // namespace a2de
