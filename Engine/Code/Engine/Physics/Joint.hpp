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

    void attach(RigidBody* a, RigidBody* b) noexcept;
    void detach(RigidBody* body) noexcept;
    void detach_all() noexcept;
    bool is_not_attached() const noexcept;

    virtual void notify([[maybe_unused]]TimeUtils::FPSeconds deltaSeconds) noexcept = 0;
    virtual void DebugRender(Renderer& renderer) const noexcept = 0;

    RigidBody* GetBodyA() const noexcept;
    RigidBody* GetBodyB() const noexcept;
protected:
    RigidBody* bodyA{};
    RigidBody* bodyB{};
private:
    virtual bool ConstraintViolated() const noexcept = 0;
    virtual void SolvePositionConstraint() const noexcept = 0;
    virtual void SolveVelocityConstraint() const noexcept = 0;

    friend class PhysicsSystem;
};
