#include "Engine/Networking/Address.hpp"

namespace Net {

Address::Address(const std::string& /*value*/) noexcept {
    //TODO: Implement Address constructor from string
}

bool Address::operator!=(const Address& rhs) const noexcept {
    return !(*this == rhs);
}

bool Address::operator==(const Address& rhs) const noexcept {
    return address.ipv4 == rhs.address.ipv4 && address.ipv6 == rhs.address.ipv6 && port == rhs.port;
}

}