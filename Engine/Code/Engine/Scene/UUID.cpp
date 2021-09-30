#include "Engine/Scene/UUID.hpp"

#include "Engine/Math/MathUtils.hpp"

#include <random>

UUID::UUID() noexcept {
    static thread_local std::random_device rd{};
    static thread_local std::mt19937_64 e{rd()};
    m_UUID = std::uniform_int_distribution<uint64_t>{}(e);
}

const uint64_t& UUID::GetID() const noexcept {
    return m_UUID;
}
