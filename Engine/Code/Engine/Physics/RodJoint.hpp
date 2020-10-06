#pragma once

#include "Engine/Math/Vector2.hpp"
#include "Engine/Physics/Joint.hpp"

struct RodJointDef : public JointDef {
    RodJointDef() = default;
    float length{};
};

class RodJoint : public Joint {
public:
    RodJoint() = delete;
    explicit RodJoint(const RodJointDef& def) noexcept;
    RodJoint(const RodJoint& other) = default;
    RodJoint(RodJoint&& other) = default;
    RodJoint& operator=(const RodJoint& other) = default;
    RodJoint& operator=(RodJoint&& other) = default;
    virtual ~RodJoint() = default;

    void Notify([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept override;
    void DebugRender(Renderer& renderer) const noexcept override;

    bool IsNotAttached() const noexcept override;
    void Attach(RigidBody* a, RigidBody* b, Vector2 localAnchorA = Vector2::ZERO, Vector2 localAnchorB = Vector2::ZERO) noexcept override;
    void Detach(const RigidBody* body) noexcept override;
    void DetachAll() noexcept override;

    RigidBody* GetBodyA() const noexcept override;
    RigidBody* GetBodyB() const noexcept override;

    Vector2 GetAnchorA() const noexcept override;
    Vector2 GetAnchorB() const noexcept override;

    float GetMassA() const noexcept override;
    float GetMassB() const noexcept override;

protected:
private:
    bool ConstraintViolated() const noexcept override;
    void SolvePositionConstraint() const noexcept override;
    void SolveVelocityConstraint() const noexcept override;

    RodJointDef _def{};

    friend class PhysicsSystem;
};
