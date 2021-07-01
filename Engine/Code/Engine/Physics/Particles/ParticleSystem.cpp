#include "Engine/Physics/Particles/ParticleSystem.hpp"

void ParticleSystem::BeginFrame() noexcept {
}

void ParticleSystem::Update([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {
}

void ParticleSystem::Render() const noexcept {
}

void ParticleSystem::EndFrame() noexcept {
}

void ParticleSystem::AddEmitter(std::unique_ptr<Emitter> newEmitter) noexcept {
    m_emitters.emplace_back(newEmitter);
}

void ParticleSystem::DestroyEmitter(Emitter* emitter) noexcept {
    if(emitter == nullptr) {
        return;
    }
    m_emitters.erase(std::remove_if(std::begin(m_emitters), std::end(m_emitters), [&emitter](const auto&& e) { return e.get() == emitter; }), std::end(m_emitters));
}

void ParticleSystem::DestroyAllEmitters() noexcept {
    m_emitters.clear();
    m_emitters.shrink_to_fit();
}
