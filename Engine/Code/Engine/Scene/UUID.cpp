#include "Engine/Scene/UUID.hpp"

#include "Engine/Math/MathUtils.hpp"

#include <random>

UUID::UUID() noexcept {
    static std::random_device rd{};
    static std::mt19937_64 e{rd()};
    id = std::uniform_int_distribution<uint64_t>{}(e);
}

const uint64_t& UUID::GetID() const noexcept {
    return id;
}
