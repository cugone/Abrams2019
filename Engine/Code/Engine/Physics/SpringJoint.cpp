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

void SpringJoint::notify([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {
    auto* first_body = bodyA;
    auto* second_body = bodyB;
    if(first_body == nullptr && second_body == nullptr) {
        return;
    }

    const auto fb_pos = first_body == nullptr ? _anchors.first : first_body->GetPosition();
    const auto sb_pos = second_body == nullptr ? _anchors.second : second_body->GetPosition();

    auto left_direction = fb_pos - sb_pos;
    auto left_magnitude = left_direction.CalcLength();
    if(MathUtils::IsEquivalent(std::abs(left_magnitude), _restingLength) == false) {
        auto current_compression = left_magnitude - _restingLength;
        if(current_compression <= -_compressionLength)
            current_compression = -_compressionLength;
        left_magnitude = _k * current_compression;
        left_direction.Normalize();
    }

    auto right_direction = sb_pos - fb_pos;
    auto right_magnitude = right_direction.CalcLength();
    if(MathUtils::IsEquivalent(std::abs(right_magnitude), _restingLength) == false) {
        auto current_compression = right_magnitude - _restingLength;
        if(current_compression <= -_compressionLength)
            current_compression = -_compressionLength;
        right_magnitude = _k * current_compression;
        right_direction.Normalize();
    }

    first_body->ApplyForce(right_direction * right_magnitude, TimeUtils::FPSeconds::zero());
    second_body->ApplyForce(left_direction * left_magnitude, TimeUtils::FPSeconds::zero());

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
