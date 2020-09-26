#pragma once

#include "Engine/Physics/Joint.hpp"
#include "Engine/Math/Vector2.hpp"

class Renderer;

class SpringJoint : public Joint {
public:
    SpringJoint() = default;
    SpringJoint(const SpringJoint& other) = default;
    SpringJoint(SpringJoint&& other) = default;
    SpringJoint& operator=(const SpringJoint& other) = default;
    SpringJoint& operator=(SpringJoint&& other) = default;
    virtual ~SpringJoint() = default;

    void SetAnchors(const Vector2& a, const Vector2& b) noexcept;
    void SetStiffness(float k) noexcept;
    void SetMinimumCompressionDistance(float distance) noexcept;
    void SetRestingLength(float length) noexcept;

    void notify([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept override;
    void DebugRender(Renderer& renderer) const noexcept override;

protected:
private:
    float _compressionLength = 0.0f;
    float _restingLength = 1.0f;
    float _k = 1.0f;
    std::pair<Vector2, Vector2> _anchors{};

    bool ConstraintViolated() const noexcept override;
    void SolvePositionConstraint() const noexcept override;
    void SolveVelocityConstraint() const noexcept override;

    friend class PhysicsSystem;
};
