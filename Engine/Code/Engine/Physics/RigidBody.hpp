#pragma once

#include "Engine/Math/Vector2.hpp"
#include "Engine/Physics/PhysicsTypes.hpp"
#include "Engine/Physics/Collider.hpp"
#include "Engine/Core/TimeUtils.hpp"

#include <memory>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable : 26444) // 143 Avoid unnamed objects with custom construction and destruction (es.84).
#endif

struct RigidBodyDesc {
    Vector2 initialPosition = Vector2::ZERO;
    Vector2 initialVelocity = Vector2::ZERO;
    Vector2 initialAcceleration = Vector2::ZERO;
    std::unique_ptr<Collider> collider = std::make_unique<ColliderOBB>(Vector2::ZERO, Vector2::ONE * 0.5f);
    PhysicsMaterial physicsMaterial = PhysicsMaterial{};
    PhysicsDesc physicsDesc = PhysicsDesc{};
    RigidBodyDesc() = default;
    RigidBodyDesc(const RigidBodyDesc& other) = delete;
    RigidBodyDesc(RigidBodyDesc&& other) = default;
    RigidBodyDesc& operator=(const RigidBodyDesc& other) = delete;
    RigidBodyDesc& operator=(RigidBodyDesc&& other) = default;
};

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

struct PhysicsSystemDesc;

class RigidBody {
public:
    explicit RigidBody([[maybe_unused]] const PhysicsSystemDesc& physicsDesc, RigidBodyDesc&& desc = RigidBodyDesc{});
    Matrix4 transform{};

    void BeginFrame();
    void Update(TimeUtils::FPSeconds deltaSeconds);
    void DebugRender(Renderer& renderer) const;
    void Endframe();

    void EnablePhysics(bool enabled);
    void EnableGravity(bool enabled);
    void EnableDrag(bool enabled);
    bool IsPhysicsEnabled() const;
    bool IsGravityEnabled() const;
    bool IsDragEnabled() const;

    void SetAwake(bool awake) noexcept;
    void Wake() noexcept;
    void Sleep() noexcept;
    bool IsAwake() const;

    float GetMass() const;
    float GetInverseMass() const;

    Matrix4 GetParentTransform() const;

    void ApplyImpulse(const Vector2& impulse);
    void ApplyImpulse(const Vector2& direction, float magnitude);

    void ApplyForce(const Vector2& force);
    void ApplyForce(const Vector2& direction, float magnitude);

    void ApplyTorque(float force, bool asImpulse = false);
    void ApplyTorque(const Vector2& direction, float magnitude, bool asImpulse = false);

    void ApplyTorqueAt(const Vector2& position_on_object, const Vector2& force, bool asImpulse = false);
    void ApplyTorqueAt(const Vector2& position_on_object, const Vector2& direction, float magnitude, bool asImpulse = false);

    void ApplyForceAt(const Vector2& position_on_object, const Vector2& direction, float magnitude);
    void ApplyForceAt(const Vector2& position_on_object, const Vector2& force);

    void ApplyImpulseAt(const Vector2& position_on_object, const Vector2& direction, float magnitude);
    void ApplyImpulseAt(const Vector2& position_on_object, const Vector2& force);

    const OBB2 GetBounds() const;

    void SetPosition(const Vector2& newPosition, bool teleport = false) noexcept;
    const Vector2& GetPosition() const;
    Vector2 GetVelocity() const;
    const Vector2& GetAcceleration() const;
    Vector2 CalcDimensions() const;
    float GetOrientationDegrees() const;
    float GetAngularVelocityDegrees() const;
    float GetAngularAccelerationDegrees() const;

    const Collider* GetCollider() const noexcept;
    Collider* GetCollider() noexcept;

protected:
private:
    RigidBodyDesc rigidbodyDesc{};
    RigidBody* parent = nullptr;
    std::vector<RigidBody*> children{};
    Vector2 prev_position{};
    Vector2 position{};
    Vector2 acceleration{};
    float prev_orientationDegrees = 0.0f;
    float orientationDegrees = 0.0f;
    float angular_acceleration = 0.0f;
    TimeUtils::FPSeconds dt{};
    TimeUtils::FPSeconds time_since_last_move{};
    std::vector<Vector2> linear_forces{};
    std::vector<Vector2> linear_impulses{};
    std::vector<float> angular_forces{};
    std::vector<float> angular_impulses{};
    bool is_colliding = false;
    bool is_awake = true;

    friend class PhysicsSystem;
};
