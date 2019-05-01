#pragma once

#include "Engine/Networking/Address.hpp"

namespace Net {

bool Initialize();
Address GetIpv4(Address& addy);
Address GetIpv6(Address& addy);
void Shutdown();

}