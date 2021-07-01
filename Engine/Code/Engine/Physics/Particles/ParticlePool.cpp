#include "Engine/Physics/Particles/ParticlePool.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

ParticlePool::ParticlePool(Emitter* emitter) noexcept {
    GUARANTEE_OR_DIE(emitter, "ParticlePool: emitter not valid.");

}

ParticlePool::~ParticlePool() noexcept {

}

void ParticlePool::Generate(const Vector3& position, const Vector3& velocity) noexcept {

}

bool ParticlePool::Update(TimeUtils::FPSeconds deltaSeconds) noexcept {

}
