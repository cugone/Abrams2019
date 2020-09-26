#pragma once

#include "Engine/Core/TimeUtils.hpp"

#include "Engine/Math/Vector2.hpp"

#include "Engine/Physics/Joint.hpp"

class CableJoint : public Joint {
public:
    CableJoint() = default;
    CableJoint(const CableJoint& other) = default;
    CableJoint(CableJoint&& other) = default;
    CableJoint& operator=(const CableJoint& other) = default;
    CableJoint& operator=(CableJoint&& other) = default;
    virtual ~CableJoint() = default;

    void attach(RigidBody* a, RigidBody* b) noexcept;
    void notify([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept override;
    void DebugRender(Renderer& renderer) const noexcept override;

protected:
private:
    void attachA(RigidBody* a) noexcept;
    void attachB(RigidBody* b) noexcept;

    bool ConstraintViolated() const noexcept override;
    void SolvePositionConstraint() const noexcept override;
    void SolveVelocityConstraint() const noexcept override;

    std::pair<Vector2, Vector2> _anchors{};
    float _length{};

    friend class PhysicsSystem;
};
