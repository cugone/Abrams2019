#include "Engine/Physics/Particles/Particle.hpp"

void Particle::BeginFrame() noexcept {

}

void Particle::Update([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {
    auto accel = CalcAcceleration();
    auto vel = GetVelocity();
    auto pos = GetPosition();
    const auto r = GetCosmeticRadius();
    Vector3 new_accel = accel;
    Vector3 new_vel = vel + new_accel * deltaSeconds.count();
    Vector3 new_pos = pos + new_vel * deltaSeconds.count();
    position = new_pos;
    velocity = new_vel;
    lifetime -= deltaSeconds;
    if(lifetime.count() < 0.0f) {
        lifetime = TimeUtils::FPSeconds::zero();
    }
}

void Particle::Render() const noexcept {

}

void Particle::EndFrame() noexcept {
}
