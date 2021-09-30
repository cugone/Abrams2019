#pragma once

#include <functional>

class UUID {
public:
    UUID() noexcept;
    UUID(const UUID& other) noexcept = default;
    UUID(UUID&& other) noexcept = default;
    UUID& operator=(const UUID& other) noexcept = default;
    UUID& operator=(UUID&& other) noexcept = default;
    ~UUID() noexcept = default;

    UUID(uint64_t uuid) noexcept;

    operator uint64_t() const noexcept { return m_UUID; }

protected:
private:
    uint64_t m_UUID{};
};

namespace std {
    template<>
    struct hash<UUID> {
        std::size_t operator()(const UUID& uuid) const noexcept {
            return hash<uint64_t>()(uuid);
        }
    };
}
