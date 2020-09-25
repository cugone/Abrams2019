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

    void attach(RigidBody* a, RigidBody* b) noexcept;
    void notify([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept override;
    void DebugRender(Renderer& renderer) const noexcept override;

protected:
private:
    void SetAnchors(const Vector2& a, const Vector2& b) noexcept;

    std::pair<Vector2, Vector2> _anchors{};
    float _length{};
};
