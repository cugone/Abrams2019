#pragma once

#include "Engine/Memory/MemoryPool.hpp"

#include "Engine/Physics/Particles/Particle.hpp"

class Emitter {
public:
    Emitter() = default;
    Emitter(const Emitter& other) = default;
    Emitter(Emitter&& other) = default;
    Emitter& operator=(const Emitter& other) = default;
    Emitter& operator=(Emitter&& other) = default;
    ~Emitter() = default;

    void BeginFrame() noexcept;
    void Update(TimeUtils::FPSeconds deltaSeconds) noexcept;
    void Render() const noexcept;
    void EndFrame() noexcept;

protected:
private:
    MemoryPool<Particle, 1024> m_particles{};
};
