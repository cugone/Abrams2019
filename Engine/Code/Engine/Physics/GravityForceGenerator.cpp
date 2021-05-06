#include "Engine/Physics/GravityForceGenerator.hpp"

#include "Engine/Physics/RigidBody.hpp"

#include "Engine/Math/Vector2.hpp"

namespace a2de {

    GravityForceGenerator::GravityForceGenerator(const Vector2& gravity) noexcept
        : ForceGenerator()
        , g(gravity)
    {
        /* DO NOTHING */
    }

    void GravityForceGenerator::notify([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) const noexcept {
        if(g == Vector2::ZERO) {
            return;
        }
        for(auto* body : _observers) {
            body->ApplyForce(g, deltaSeconds);
        }
    }

    void GravityForceGenerator::SetGravity(const Vector2& newGravity) noexcept {
        g = newGravity;
    }

} // namespace a2de
