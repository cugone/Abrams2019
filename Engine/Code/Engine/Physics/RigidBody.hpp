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
    Collider* collider{nullptr};
    PhysicsMaterial physicsMaterial = PhysicsMaterial{};
    PhysicsDesc physicsDesc = PhysicsDesc{};
    RigidBodyDesc() noexcept
        : collider(new ColliderOBB(Vector2::ZERO, Vector2::ONE * 0.5f))
    {
        /* DO NOTHING */
    }
    RigidBodyDesc(const Vector2& initialPos, const Vector2& initialVel, const Vector2& initialAcc, Collider* coll, const PhysicsMaterial& physMat, const PhysicsDesc& physDesc) noexcept
        : initialPosition(initialPos)
        , initialVelocity(initialVel)
        , initialAcceleration(initialAcc)
        , collider(coll)
        , physicsMaterial(physMat)
        , physicsDesc(physDesc)
    {
        /* DO NOTHING */
    }
    RigidBodyDesc(RigidBodyDesc&& other) noexcept
        : initialPosition(std::move(other.initialPosition))
        , initialVelocity(std::move(other.initialVelocity))
        , initialAcceleration(std::move(other.initialAcceleration))
        , physicsMaterial(std::move(other.physicsMaterial))
        , physicsDesc(std::move(other.physicsDesc))
    {
        collider = std::move(other.collider);
        other.collider = nullptr;
    }
    RigidBodyDesc& operator=(RigidBodyDesc&& other) noexcept {
        initialPosition = std::move(other.initialPosition);
        initialVelocity = std::move(other.initialVelocity);
        initialAcceleration = std::move(other.initialAcceleration);
        physicsMaterial = std::move(other.physicsMaterial);
        physicsDesc = std::move(other.physicsDesc);
        collider = std::move(other.collider);
        other.collider = nullptr;
        return *this;
    }

    RigidBodyDesc(const RigidBodyDesc& other) noexcept
    : initialPosition(other.initialPosition)
    ,initialVelocity(other.initialVelocity)
    ,initialAcceleration(other.initialAcceleration)
    ,physicsMaterial(other.physicsMaterial)
    ,physicsDesc(other.physicsDesc)
    {
        auto new_collider = other.collider->Clone();
        delete collider;
        collider = new_collider;
    }
    RigidBodyDesc& operator=(const RigidBodyDesc& other) noexcept {
        initialPosition = other.initialPosition;
        initialVelocity = other.initialVelocity;
        initialAcceleration = other.initialAcceleration;
        physicsMaterial = other.physicsMaterial;
        physicsDesc = other.physicsDesc;
        auto new_collider = other.collider->Clone();
        delete collider;
        collider = new_collider;
        return *this;
    }
    ~RigidBodyDesc() noexcept {
        delete collider;
        collider = nullptr;
    }
};

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

struct PhysicsSystemDesc;
class PhysicsSystem;

class RigidBody {
public:
    explicit RigidBody(PhysicsSystem* physicsSystem, const RigidBodyDesc& desc = RigidBodyDesc{});

    RigidBody() = delete;
    RigidBody(RigidBody&& other) noexcept = default;
    RigidBody& operator=(RigidBody&& rhs) noexcept = default;

    RigidBody(const RigidBody& other) noexcept = default;
    RigidBody& operator=(const RigidBody& rhs) noexcept = default;
    ~RigidBody() noexcept = default;

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

    void FellOutOfWorld() noexcept;
    const bool ShouldKill() const noexcept;
protected:
private:
    PhysicsSystem* parentPhysicsSystem{nullptr};
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
    bool should_kill = false;

    friend class PhysicsSystem;
};
