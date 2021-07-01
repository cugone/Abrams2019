#pragma once

#include "Engine/Core/TimeUtils.hpp"
#include "Engine/Math/Vector3.hpp"

#include <cstddef>

class Particle;
class Emitter;

class ParticlePool {
public:
    explicit ParticlePool(Emitter* emitter) noexcept;
    virtual ~ParticlePool() noexcept;

    void Generate(const Vector3& position, const Vector3& velocity) noexcept;
    bool Update(TimeUtils::FPSeconds deltaSeconds) noexcept;
protected:
private:
    std::size_t m_size{0u};
    Particle* m_first{};
    Particle* m_array{};
};
