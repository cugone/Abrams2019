#pragma once

#include "Engine/Core/TimeUtils.hpp"

#include "Engine/Math/Vector2.hpp"

#include "Engine/Physics/Joint.hpp"

struct CableJointDef : public JointDef {
    CableJointDef() = default;
    float length{};
};

class CableJoint : public Joint {
public:
    CableJoint() = delete;
    explicit CableJoint(const CableJointDef& def) noexcept;
    CableJoint(const CableJoint& other) = default;
    CableJoint(CableJoint&& other) = default;
    CableJoint& operator=(const CableJoint& other) = default;
    CableJoint& operator=(CableJoint&& other) = default;
    virtual ~CableJoint() = default;

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
    bool ConstraintViolated() const noexcept override;
    void SolvePositionConstraint() const noexcept override;
    void SolveVelocityConstraint() const noexcept override;

    CableJointDef _def{};

    friend class PhysicsSystem;
};
