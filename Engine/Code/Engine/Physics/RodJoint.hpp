#pragma once

#include "Engine/Math/Vector2.hpp"
#include "Engine/Physics/Joint.hpp"

class RodJoint : public Joint {
public:
    RodJoint() = default;
    RodJoint(const RodJoint& other) = default;
    RodJoint(RodJoint&& other) = default;
    RodJoint& operator=(const RodJoint& other) = default;
    RodJoint& operator=(RodJoint&& other) = default;
    virtual ~RodJoint() = default;

    void Attach(RigidBody* a, RigidBody* b) noexcept;
    void Notify([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept override;
    void DebugRender(Renderer& renderer) const noexcept override;
    void SetAnchors(const Vector2& a, const Vector2& b) noexcept;

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
