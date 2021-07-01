#pragma once

#include "Engine/Core/TimeUtils.hpp"

#include "Engine/Physics/Particles/Emitter.hpp"

#include <memory>
#include <vector>

class ParticleSystem {
public:
    ParticleSystem() = default;
    ParticleSystem(const ParticleSystem& other) = default;
    ParticleSystem(ParticleSystem&& other) = default;
    ParticleSystem& operator=(const ParticleSystem& other) = default;
    ParticleSystem& operator=(ParticleSystem&& other) = default;
    ~ParticleSystem() = default;

    void BeginFrame() noexcept;
    void Update(TimeUtils::FPSeconds deltaSeconds) noexcept;
    void Render() const noexcept;
    void EndFrame() noexcept;

    void AddEmitter(std::unique_ptr<Emitter> newEmitter) noexcept;
    void DestroyEmitter(Emitter* emitter) noexcept;
    void DestroyAllEmitters() noexcept;

protected:
private:
    std::vector<std::unique_ptr<Emitter>> m_emitters{};
};
