#include "Engine/Physics/SpringJoint.hpp"

#include "Engine/Physics/RigidBody.hpp"
#include "Engine/Physics/PhysicsUtils.hpp"

#include "Engine/Renderer/Renderer.hpp"

SpringJoint::SpringJoint(const SpringJointDef& def) noexcept {
    _def.type = def.type;
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
    _def.k = def.k;
    _def.restingLength = def.restingLength;
}

void SpringJoint::Notify([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {
    auto* first_body = _def.rigidBodyA;
    auto* second_body = _def.rigidBodyB;
    if(first_body == nullptr && second_body == nullptr) {
        return;
    }

    const auto fb_pos = _def.worldAnchorA;
    const auto sb_pos = _def.worldAnchorB;

    const auto displacement_towards_first = fb_pos - sb_pos;
    const auto direction_towards_first = displacement_towards_first.GetNormalize();
    auto towards_first_magnitude = displacement_towards_first.CalcLength();
    if(MathUtils::IsEquivalent(towards_first_magnitude, _def.restingLength) == false) {
        const auto spring_displacement = towards_first_magnitude - _def.restingLength;
        towards_first_magnitude = _def.k * spring_displacement;
    }

    const auto displacement_towards_second = sb_pos - fb_pos;
    const auto direction_towards_second = displacement_towards_second.GetNormalize();
    auto towards_second_magnitude = displacement_towards_second.CalcLength();
    if(MathUtils::IsEquivalent(towards_second_magnitude, _def.restingLength) == false) {
        auto spring_displacement = towards_second_magnitude - _def.restingLength;
        towards_second_magnitude = _def.k * spring_displacement;
    }

    first_body->ApplyImpulse(direction_towards_second * towards_second_magnitude);
    second_body->ApplyImpulse(direction_towards_first * towards_first_magnitude);

}

void SpringJoint::DebugRender(Renderer& renderer) const noexcept {
    auto* first_body = _def.rigidBodyA;
    auto* second_body = _def.rigidBodyB;
    if(!(first_body || second_body)) {
        return;
    }
    const auto fb_pos = first_body == nullptr ? _def.localAnchorA : first_body->GetPosition() + first_body->CalcDimensions() * 0.5f * _def.localAnchorA;
    const auto sb_pos = second_body == nullptr ? _def.localAnchorB : second_body->GetPosition() + second_body->CalcDimensions() * 0.5f * _def.localAnchorB;
    renderer.SetModelMatrix(Matrix4::I);
    renderer.DrawLine2D(fb_pos, sb_pos);
}

void SpringJoint::Attach(RigidBody* a, RigidBody* b, Vector2 localAnchorA /*= Vector2::ZERO*/, Vector2 localAnchorB /*= Vector2::ZERO*/) noexcept {
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

void SpringJoint::Detach(RigidBody* body) noexcept {
    if(body == _def.rigidBodyA) {
        _def.rigidBodyA = nullptr;
    } else if(body == _def.rigidBodyB) {
        _def.rigidBodyB = nullptr;
    }
}

void SpringJoint::DetachAll() noexcept {
    _def.rigidBodyA = nullptr;
    _def.rigidBodyB = nullptr;
}

bool SpringJoint::IsNotAttached() const noexcept {
    return _def.rigidBodyA == nullptr || _def.rigidBodyB == nullptr;
}

RigidBody* SpringJoint::GetBodyA() const noexcept {
    return _def.rigidBodyA;
}

RigidBody* SpringJoint::GetBodyB() const noexcept {
    return _def.rigidBodyB;
}

Vector2 SpringJoint::GetAnchorA() const noexcept {
    return _def.rigidBodyA ? _def.rigidBodyA->GetPosition() + (_def.rigidBodyA->CalcDimensions() * 0.5f * _def.localAnchorA) : _def.worldAnchorA;
}

Vector2 SpringJoint::GetAnchorB() const noexcept {
    return _def.rigidBodyB ? _def.rigidBodyB->GetPosition() + (_def.rigidBodyB->CalcDimensions() * 0.5f * _def.localAnchorB) : _def.worldAnchorB;
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
