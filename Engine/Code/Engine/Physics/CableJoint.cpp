#include "Engine/Physics/CableJoint.hpp"

#include "Engine/Math/MathUtils.hpp"

#include "Engine/Physics/RigidBody.hpp"

#include "Engine/Renderer/Renderer.hpp"

void CableJoint::attach(RigidBody* a, RigidBody* b) noexcept {
    auto posA = Vector2::ZERO;
    auto posB = Vector2::ZERO;
    if(a) {
        attachA(a);
        posA = a->GetPosition();
    }
    if(b) {
        attachB(b);
        posB = b->GetPosition();
    }
    _length = MathUtils::CalcDistance(posA, posB);
}

void CableJoint::attachA(RigidBody* a) noexcept {
    bodyA = a;
}

void CableJoint::attachB(RigidBody* b) noexcept {
    bodyB = b;
}

void CableJoint::notify([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {
    auto* first_body = bodyA;
    auto* second_body = bodyB;
    if(first_body == nullptr && second_body == nullptr) {
        return;
    }

    const auto fb_pos = first_body == nullptr ? _anchors.first : first_body->GetPosition();
    const auto sb_pos = second_body == nullptr ? _anchors.second : second_body->GetPosition();

    const auto distance = MathUtils::CalcDistance(fb_pos, sb_pos);
    const auto displacement_towards_first = fb_pos - sb_pos;
    const auto displacement_towards_second = sb_pos - fb_pos;
    const auto direction_to_first = displacement_towards_first.GetNormalize();
    const auto direction_to_second = displacement_towards_second.GetNormalize();
    const auto m1 = (first_body ? first_body->GetMass() : 0.0f);
    const auto m2 = (second_body ? second_body->GetMass() : 0.0f);
    const auto mass_sum = m1 + m2;
    const auto mass1_ratio = m1 / mass_sum;
    const auto mass2_ratio = m2 / mass_sum;

    if(_length < distance) {
        if(first_body) {
            first_body->ApplyImpulse(direction_to_second * mass1_ratio);
        }
        if(second_body) {
            second_body->ApplyImpulse(direction_to_first * mass2_ratio);
        }
    }
}

void CableJoint::DebugRender(Renderer& renderer) const noexcept {
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

bool CableJoint::ConstraintViolated() const noexcept {
    const bool violated = [this]() -> const bool {
        const auto* bodyA = GetBodyA();
        const auto* bodyB = GetBodyB();
        const auto posA = bodyA ? bodyA->GetPosition() : _anchors.first;
        const auto posB = bodyB ? bodyB->GetPosition() : _anchors.second;
        const auto distance = MathUtils::CalcDistance(posA, posB);
        return _length < distance;
    }();
    return violated;
}

void CableJoint::SolvePositionConstraint() const noexcept {
    auto* first_body = bodyA;
    auto* second_body = bodyB;
    if(first_body == nullptr && second_body == nullptr) {
        return;
    }
    const auto fb_pos = first_body == nullptr ? _anchors.first : first_body->GetPosition();
    const auto sb_pos = second_body == nullptr ? _anchors.second : second_body->GetPosition();

    const auto distance = MathUtils::CalcDistance(fb_pos, sb_pos);
    const auto displacement_towards_first = fb_pos - sb_pos;
    const auto displacement_towards_second = sb_pos - fb_pos;
    const auto direction_to_first = displacement_towards_first.GetNormalize();
    const auto direction_to_second = displacement_towards_second.GetNormalize();
    const auto m1 = (first_body ? first_body->GetMass() : 0.0f);
    const auto m2 = (second_body ? second_body->GetMass() : 0.0f);
    const auto mass_sum = m1 + m2;
    const auto mass1_ratio = m1 / mass_sum;
    const auto mass2_ratio = m2 / mass_sum;
    auto newPosition1 = fb_pos;
    auto newPosition2 = sb_pos;

    if(_length < distance) {
        if(first_body) {
            const auto newDisplacement = mass1_ratio * direction_to_second * std::abs(_length - distance);
            const auto newPosition = first_body->GetPosition() + newDisplacement;
            newPosition1 = newPosition;
        }
        if(second_body) {
            const auto newDisplacement = mass2_ratio * direction_to_first * std::abs(_length - distance);
            const auto newPosition = second_body->GetPosition() + newDisplacement;
            newPosition2 = newPosition;
        }
    }
    if(first_body) {
        first_body->SetPosition(newPosition1, true);
    }
    if(second_body) {
        second_body->SetPosition(newPosition2, true);
    }
}

void CableJoint::SolveVelocityConstraint() const noexcept {
    auto* first_body = bodyA;
    auto* second_body = bodyB;
    if(first_body == nullptr && second_body == nullptr) {
        return;
    }

    const auto fb_pos = first_body == nullptr ? _anchors.first : first_body->GetPosition();
    const auto sb_pos = second_body == nullptr ? _anchors.second : second_body->GetPosition();

    const auto distance = MathUtils::CalcDistance(fb_pos, sb_pos);
    const auto displacement_towards_first = fb_pos - sb_pos;
    const auto displacement_towards_second = sb_pos - fb_pos;
    const auto direction_to_first = displacement_towards_first.GetNormalize();
    const auto direction_to_second = displacement_towards_second.GetNormalize();
    const auto m1 = (first_body ? first_body->GetMass() : 0.0f);
    const auto m2 = (second_body ? second_body->GetMass() : 0.0f);
    const auto mass_sum = m1 + m2;
    const auto mass1_ratio = m1 / mass_sum;
    const auto mass2_ratio = m2 / mass_sum;
    auto v1 = first_body ? first_body->GetVelocity() : Vector2::ZERO;
    auto v2 = second_body ? second_body->GetVelocity() : Vector2::ZERO;
    auto newVelocity1 = v1;
    auto newVelocity2 = v2;

    if(_length < distance) { //Extension
        newVelocity1 = mass1_ratio * MathUtils::Reject(v1, direction_to_second);
        newVelocity2 = mass2_ratio * MathUtils::Reject(v2, direction_to_first);
    }
    if(first_body) {
        first_body->SetVelocity(newVelocity1);
    }
    if(second_body) {
        second_body->SetVelocity(newVelocity2);
    }
}
