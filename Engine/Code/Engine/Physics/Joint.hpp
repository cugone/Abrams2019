#pragma once

#include "Engine/Core/TimeUtils.hpp"

#include "Engine/Math/Vector2.hpp"

class RigidBody;
class Renderer;

struct JointDef {
    virtual ~JointDef() = default;
    RigidBody* rigidBodyA{};
    RigidBody* rigidBodyB{};
    Vector2 localAnchorA{};
    Vector2 localAnchorB{};
    Vector2 worldAnchorA{};
    Vector2 worldAnchorB{};
    Vector2 breakForce{};
    Vector2 breakTorque{};
    float linearDamping{1.0f};
    float angularDamping{1.0f};
    bool attachedCollidable{false};
};

class Joint {
public:
    Joint() = default;
    Joint(const Joint& other) = default;
    Joint(Joint&& other) = default;
    Joint& operator=(const Joint& other) = default;
    Joint& operator=(Joint&& other) = default;
    virtual ~Joint() = default;

    virtual void Attach(RigidBody* a, RigidBody* b, Vector2 localAnchorA = Vector2::ZERO, Vector2 localAnchorB = Vector2::ZERO) noexcept = 0;
    virtual void Detach(const RigidBody* body) noexcept = 0;
    virtual void DetachAll() noexcept = 0;
    virtual bool IsNotAttached() const noexcept = 0;

    virtual void Notify([[maybe_unused]]TimeUtils::FPSeconds deltaSeconds) noexcept = 0;
    virtual void DebugRender(Renderer& renderer) const noexcept = 0;

    virtual RigidBody* GetBodyA() const noexcept = 0;
    virtual RigidBody* GetBodyB() const noexcept = 0;
    virtual Vector2 GetAnchorA() const noexcept = 0;
    virtual Vector2 GetAnchorB() const noexcept = 0;

protected:
private:
    virtual bool ConstraintViolated() const noexcept = 0;
    virtual void SolvePositionConstraint() const noexcept = 0;
    virtual void SolveVelocityConstraint() const noexcept = 0;

    friend class PhysicsSystem;
};
