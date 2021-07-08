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
    void DebugRender() const noexcept override;

    [[nodiscard]] bool IsNotAttached() const noexcept override;
    void Attach(RigidBody* a, RigidBody* b, Vector2 localAnchorA = Vector2::ZERO, Vector2 localAnchorB = Vector2::ZERO) noexcept override;
    void Detach(const RigidBody* body) noexcept override;
    void DetachAll() noexcept override;

    [[nodiscard]] RigidBody* GetBodyA() const noexcept override;
    [[nodiscard]] RigidBody* GetBodyB() const noexcept override;

    [[nodiscard]] Vector2 GetAnchorA() const noexcept override;
    [[nodiscard]] Vector2 GetAnchorB() const noexcept override;

    [[nodiscard]] float GetMassA() const noexcept override;
    [[nodiscard]] float GetMassB() const noexcept override;

protected:
private:
    [[nodiscard]] bool ConstraintViolated() const noexcept override;
    void SolvePositionConstraint() const noexcept override;
    void SolveVelocityConstraint() const noexcept override;

    RodJointDef _def{};

    friend class PhysicsSystem;
};
