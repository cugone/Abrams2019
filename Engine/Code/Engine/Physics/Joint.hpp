#pragma once

#include "Engine/Core/TimeUtils.hpp"

class RigidBody;
class Renderer;

class Joint {
public:
    Joint() = default;
    Joint(const Joint& other) = default;
    Joint(Joint&& other) = default;
    Joint& operator=(const Joint& other) = default;
    Joint& operator=(Joint&& other) = default;
    virtual ~Joint() = default;

    void Attach(RigidBody* a, RigidBody* b) noexcept;
    void Detach(RigidBody* body) noexcept;
    void DetachAll() noexcept;
    bool IsNotAttached() const noexcept;

    void AttachedCanCollide(bool canCollide) noexcept;

    virtual void Notify([[maybe_unused]]TimeUtils::FPSeconds deltaSeconds) noexcept = 0;
    virtual void DebugRender(Renderer& renderer) const noexcept = 0;

    RigidBody* GetBodyA() const noexcept;
    RigidBody* GetBodyB() const noexcept;

protected:
    RigidBody* bodyA{};
    RigidBody* bodyB{};
    bool attachedCanCollide{false};
private:
    virtual bool ConstraintViolated() const noexcept = 0;
    virtual void SolvePositionConstraint() const noexcept = 0;
    virtual void SolveVelocityConstraint() const noexcept = 0;

    friend class PhysicsSystem;
};
