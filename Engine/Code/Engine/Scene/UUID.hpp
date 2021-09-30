#pragma once

#include <cstdint>

class UUID {
public:
    UUID() noexcept;
    UUID(const UUID& other) noexcept = default;
    UUID(UUID&& other) noexcept = default;
    UUID& operator=(const UUID& other) noexcept = default;
    UUID& operator=(UUID&& other) noexcept = default;
    ~UUID() noexcept = default;

    const uint64_t& GetID() const noexcept;

protected:
private:
    uint64_t m_UUID{};
};
