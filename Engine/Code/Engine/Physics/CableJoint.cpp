#include "Engine/Physics/CableJoint.hpp"

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Physics/PhysicsUtils.hpp"
#include "Engine/Physics/RigidBody.hpp"
#include "Engine/Renderer/Renderer.hpp"

CableJoint::CableJoint(const CableJointDef& def) noexcept {
    _def.rigidBodyA = def.rigidBodyA;
    _def.rigidBodyB = def.rigidBodyB;
    _def.localAnchorA = def.localAnchorA;
    _def.localAnchorB = def.localAnchorB;
    _def.linearDamping = def.linearDamping;
    _def.angularDamping = def.angularDamping;
    _def.attachedCollidable = def.attachedCollidable;
    _def.breakForce = def.breakForce;
    _def.breakTorque = def.breakTorque;
    auto posA = _def.localAnchorA;
    auto posB = _def.localAnchorB;
    if(_def.rigidBodyA) {
        posA = _def.rigidBodyA->GetPosition() + (_def.rigidBodyA->CalcDimensions() * 0.5f * _def.localAnchorA);
    }
    if(_def.rigidBodyB) {
        posB = _def.rigidBodyB->GetPosition() + (_def.rigidBodyB->CalcDimensions() * 0.5f * _def.localAnchorB);
    }
    _def.worldAnchorA = posA;
    _def.worldAnchorB = posB;
    _def.length = def.length;
}

void CableJoint::Notify([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {
    auto* first_body = GetBodyA();
    auto* second_body = GetBodyB();
    if(first_body == nullptr && second_body == nullptr) {
        return;
    }

    const auto fb_pos = GetAnchorA();
    const auto sb_pos = GetAnchorB();

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
    const auto length = _def.length;
    if(length < distance) {
        if(first_body) {
            first_body->ApplyImpulse(direction_to_second * mass1_ratio);
        }
        if(second_body) {
            second_body->ApplyImpulse(direction_to_first * mass2_ratio);
        }
    }
}

void CableJoint::DebugRender(Renderer& renderer) const noexcept {
    if(!(_def.rigidBodyA || _def.rigidBodyB)) {
        return;
    }
    const auto posA = GetAnchorA();
    const auto posB = GetAnchorB();
    renderer.SetModelMatrix(Matrix4::I);
    renderer.DrawLine2D(posA, posB);
}

void CableJoint::Attach(RigidBody* a, RigidBody* b, Vector2 localAnchorA /*= Vector2::ZERO*/, Vector2 localAnchorB /*= Vector2::ZERO*/) noexcept {
    _def.rigidBodyA = a;
    _def.rigidBodyB = b;
    _def.localAnchorA = localAnchorA;
    _def.localAnchorB = localAnchorB;
    if(a) {
        _def.worldAnchorA = _def.rigidBodyA->GetPosition() + (_def.rigidBodyA->CalcDimensions() * 0.5f * _def.localAnchorA);
    }
    if(b) {
        _def.worldAnchorB = _def.rigidBodyB->GetPosition() + (_def.rigidBodyB->CalcDimensions() * 0.5f * _def.localAnchorB);
    }
}

void CableJoint::Detach(const RigidBody* body) noexcept {
    if(body == _def.rigidBodyA) {
        _def.rigidBodyA = nullptr;
    } else if(body == _def.rigidBodyB) {
        _def.rigidBodyB = nullptr;
    }
}

void CableJoint::DetachAll() noexcept {
    _def.rigidBodyA = nullptr;
    _def.rigidBodyB = nullptr;
}

bool CableJoint::IsNotAttached() const noexcept {
    return _def.rigidBodyA == nullptr || _def.rigidBodyB == nullptr;
}

RigidBody* CableJoint::GetBodyA() const noexcept {
    return _def.rigidBodyA;
}

RigidBody* CableJoint::GetBodyB() const noexcept {
    return _def.rigidBodyB;
}

Vector2 CableJoint::GetAnchorA() const noexcept {
    return _def.rigidBodyA ? _def.rigidBodyA->GetPosition() + (_def.rigidBodyA->CalcDimensions() * 0.5f * _def.localAnchorA) : _def.worldAnchorA;
}

Vector2 CableJoint::GetAnchorB() const noexcept {
    return _def.rigidBodyB ? _def.rigidBodyB->GetPosition() + (_def.rigidBodyB->CalcDimensions() * 0.5f * _def.localAnchorB) : _def.worldAnchorB;
}

float CableJoint::GetMassA() const noexcept {
    return _def.rigidBodyA ? _def.rigidBodyA->GetMass() : 0.0f;
}

float CableJoint::GetMassB() const noexcept {
    return _def.rigidBodyB ? _def.rigidBodyB->GetMass() : 0.0f;
}

bool CableJoint::ConstraintViolated() const noexcept {
    const bool violated = [this]() -> const bool {
        const auto distance = MathUtils::CalcDistance(GetAnchorA(), GetAnchorB());
        return _def.length < distance;
    }();
    return violated;
}

void CableJoint::SolvePositionConstraint() const noexcept {
    auto* first_body = GetBodyA();
    auto* second_body = GetBodyB();
    if(first_body == nullptr && second_body == nullptr) {
        return;
    }
    const auto posA = GetAnchorA();
    const auto posB = GetAnchorB();

    const auto distance = MathUtils::CalcDistance(posA, posB);
    const auto displacement_towards_first = posA - posB;
    const auto displacement_towards_second = posB - posA;
    const auto direction_to_first = displacement_towards_first.GetNormalize();
    const auto direction_to_second = displacement_towards_second.GetNormalize();
    const auto m1 = (first_body ? first_body->GetMass() : 0.0f);
    const auto m2 = (second_body ? second_body->GetMass() : 0.0f);
    const auto mass_sum = m1 + m2;
    const auto mass1_ratio = m1 / mass_sum;
    const auto mass2_ratio = m2 / mass_sum;
    const auto length = _def.length;
    auto newPosition1 = posA;
    auto newPosition2 = posB;
    if(length < distance) {
        if(first_body) {
            const auto newDisplacement = mass1_ratio * direction_to_second * std::abs(length - distance);
            const auto newPosition = first_body->GetPosition() + newDisplacement;
            newPosition1 = newPosition;
        }
        if(second_body) {
            const auto newDisplacement = mass2_ratio * direction_to_first * std::abs(length - distance);
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
    auto* first_body = GetBodyA();
    auto* second_body = GetBodyB();
    if(first_body == nullptr && second_body == nullptr) {
        return;
    }

    const auto posA = GetAnchorA();
    const auto posB = GetAnchorB();

    const auto distance = MathUtils::CalcDistance(posA, posB);
    const auto displacement_towards_first = posA - posB;
    const auto displacement_towards_second = posB - posA;
    const auto direction_to_first = displacement_towards_first.GetNormalize();
    const auto direction_to_second = displacement_towards_second.GetNormalize();
    const auto m1 = GetMassA();
    const auto m2 = GetMassB();
    const auto mass_sum = m1 + m2;
    const auto mass1_ratio = m1 / mass_sum;
    const auto mass2_ratio = m2 / mass_sum;
    auto v1 = first_body ? first_body->GetVelocity() : Vector2::ZERO;
    auto v2 = second_body ? second_body->GetVelocity() : Vector2::ZERO;
    auto newVelocity1 = v1;
    auto newVelocity2 = v2;
    const auto length = _def.length;
    newVelocity1 = mass1_ratio * MathUtils::Reject(v1, direction_to_second);
    newVelocity2 = mass2_ratio * MathUtils::Reject(v2, direction_to_first);
    if(first_body) {
        first_body->SetVelocity(newVelocity1);
    }
    if(second_body) {
        second_body->SetVelocity(newVelocity2);
    }
}
