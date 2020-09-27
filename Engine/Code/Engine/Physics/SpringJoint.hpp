#pragma once

#include "Engine/Physics/Joint.hpp"
#include "Engine/Math/Vector2.hpp"

class Renderer;

struct SpringJointDef : public JointDef {
    SpringJointDef() { type = JointType::Spring; };
    virtual ~SpringJointDef() = default;
    float restingLength = 1.0f;
    float k = 1.0f;
};

class SpringJoint : public Joint {
public:
    SpringJoint() = delete;
    explicit SpringJoint(const SpringJointDef& def) noexcept;
    SpringJoint(const SpringJoint& other) = default;
    SpringJoint(SpringJoint&& other) = default;
    SpringJoint& operator=(const SpringJoint& other) = default;
    SpringJoint& operator=(SpringJoint&& other) = default;
    virtual ~SpringJoint() = default;

    void Notify([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept override;
    void DebugRender(Renderer& renderer) const noexcept override;
    
    bool IsNotAttached() const noexcept override;
    void Attach(RigidBody* a, RigidBody* b, Vector2 localAnchorA = Vector2::ZERO, Vector2 localAnchorB = Vector2::ZERO) noexcept override;
    void Detach(RigidBody* body) noexcept override;
    void DetachAll() noexcept override;

    RigidBody* GetBodyA() const noexcept override;
    RigidBody* GetBodyB() const noexcept override;

    Vector2 GetAnchorA() const noexcept override;
    Vector2 GetAnchorB() const noexcept override;

protected:
private:
    SpringJointDef _def{};

    bool ConstraintViolated() const noexcept override;
    void SolvePositionConstraint() const noexcept override;
    void SolveVelocityConstraint() const noexcept override;

    friend class PhysicsSystem;
};
