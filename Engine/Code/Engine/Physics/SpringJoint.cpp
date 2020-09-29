#include "Engine/Physics/SpringJoint.hpp"

#include "Engine/Physics/RigidBody.hpp"
#include "Engine/Physics/PhysicsUtils.hpp"

#include "Engine/Renderer/Renderer.hpp"

SpringJoint::SpringJoint(const SpringJointDef& def) noexcept {
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
    _def.length = def.length;
}

void SpringJoint::Notify([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {
    auto* first_body = GetBodyA();
    auto* second_body = GetBodyB();
    if(first_body == nullptr && second_body == nullptr) {
        return;
    }

    const auto posA = GetAnchorA();
    const auto posB = GetAnchorB();
    const auto velA = first_body ? first_body->GetVelocity() : Vector2::ZERO;
    const auto velB = second_body ? second_body->GetVelocity() : Vector2::ZERO;
    const auto position_displacement = posB - posA;
    const auto velocity_displacement = velB - velA;
    const auto force = _def.k * (position_displacement.CalcLength() - _def.length) * position_displacement.GetNormalize();
    const auto damping_force = _def.k * MathUtils::DotProduct(velocity_displacement, position_displacement) * (position_displacement / position_displacement.CalcLengthSquared());

    first_body->ApplyImpulse(force + damping_force);
    second_body->ApplyImpulse(-force + -damping_force);

}

void SpringJoint::DebugRender(Renderer& renderer) const noexcept {
    if(!(_def.rigidBodyA || _def.rigidBodyB)) {
        return;
    }
    const auto posA = GetAnchorA();
    const auto posB = GetAnchorB();
    renderer.SetModelMatrix(Matrix4::I);
    renderer.DrawLine2D(posA, posB);
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
