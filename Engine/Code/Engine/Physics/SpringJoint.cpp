#include "Engine/Physics/SpringJoint.hpp"

#include "Engine/Physics/RigidBody.hpp"

#include "Engine/Renderer/Renderer.hpp"

void SpringJoint::SetAnchors(const Vector2& a, const Vector2& b) noexcept {
    _anchors = std::make_pair(a, b);
}

void SpringJoint::SetStiffness(float k) noexcept {
    _k = k;
}

void SpringJoint::SetMinimumCompressionDistance(float distance) noexcept {
    _compressionLength = distance;
}

void SpringJoint::SetRestingLength(float length) noexcept {
    _restingLength = length;
}

void SpringJoint::Notify([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {
    auto* first_body = bodyA;
    auto* second_body = bodyB;
    if(first_body == nullptr && second_body == nullptr) {
        return;
    }

    const auto fb_pos = first_body == nullptr ? _anchors.first : first_body->GetPosition();
    const auto sb_pos = second_body == nullptr ? _anchors.second : second_body->GetPosition();

    const auto displacement_towards_first = fb_pos - sb_pos;
    const auto direction_towards_first = displacement_towards_first.GetNormalize();
    auto towards_first_magnitude = displacement_towards_first.CalcLength();
    if(MathUtils::IsEquivalent(towards_first_magnitude, _restingLength) == false) {
        const auto spring_displacement = towards_first_magnitude - _restingLength;
        towards_first_magnitude = _k * spring_displacement;
    }

    const auto displacement_towards_second = sb_pos - fb_pos;
    const auto direction_towards_second = displacement_towards_second.GetNormalize();
    auto towards_second_magnitude = displacement_towards_second.CalcLength();
    if(MathUtils::IsEquivalent(towards_second_magnitude, _restingLength) == false) {
        auto spring_displacement = towards_second_magnitude - _restingLength;
        towards_second_magnitude = _k * spring_displacement;
    }

    first_body->ApplyImpulse(direction_towards_second * towards_second_magnitude);
    second_body->ApplyImpulse(direction_towards_first * towards_first_magnitude);

}

void SpringJoint::DebugRender(Renderer& renderer) const noexcept {
    auto* first_body = bodyA;
    auto* second_body = bodyB;
    if(!(first_body || second_body)) {
        return;
    }
    const auto fb_pos = first_body == nullptr ? _anchors.first : first_body->GetPosition();
    const auto sb_pos = second_body == nullptr ? _anchors.second : second_body->GetPosition();
    renderer.SetModelMatrix(Matrix4::I);
    renderer.DrawLine2D(fb_pos, sb_pos);
}

bool SpringJoint::ConstraintViolated() const noexcept {
    return false;
}

void SpringJoint::SolvePositionConstraint() const noexcept {
    /* DO NOTHING */
}

void SpringJoint::SolveVelocityConstraint() const noexcept {
    /* DO NOTHING */
}
