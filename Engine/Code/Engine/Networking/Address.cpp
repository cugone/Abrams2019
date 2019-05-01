#include "Engine/Networking/Address.hpp"

namespace Net {

Address::Address(const std::string& /*value*/) {

}

bool Address::operator!=(const Address& rhs) const {
    return !(*this == rhs);
}

bool Address::operator==(const Address& rhs) const {
    return address.ipv4 == rhs.address.ipv4 && address.ipv6 == rhs.address.ipv6 && port == rhs.port;
}

}