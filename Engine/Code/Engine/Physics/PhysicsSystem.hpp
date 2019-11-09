#pragma once

#include "Engine/Core/ThreadSafeQueue.hpp"
#include "Engine/Core/TimeUtils.hpp"
#include "Engine/Core/Vertex3D.hpp"
#include "Engine/Math/Matrix4.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/Polygon2.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include "Engine/Physics/QuadTree.hpp"

#include <atomic>
#include <algorithm>
#include <condition_variable>
#include <memory>
#include <numeric>
#include <thread>
#include <tuple>
#include <vector>

class Renderer;

class Collider {
public:
    virtual ~Collider() = default;
    virtual void DebugRender(Renderer& renderer) const noexcept = 0;
    virtual Vector2 CalcDimensions() const noexcept = 0;
    virtual Vector2 CalcCenter() const noexcept = 0;
    virtual float CalcArea() const noexcept = 0;
    virtual const Vector2& GetHalfExtents() const noexcept = 0;
    virtual void SetPosition(const Vector2& position) noexcept = 0;
    virtual void SetOrientationDegrees(float orientationDegrees) noexcept = 0;
    virtual float GetOrientationDegrees() const noexcept = 0;
    virtual OBB2 GetBounds() const noexcept = 0;
    virtual Vector2 Support(const Vector2& d) const noexcept = 0;
};

class ColliderPolygon : public Collider {
public:
    ColliderPolygon();

    explicit ColliderPolygon(int sides = 4, const Vector2& position = Vector2::ZERO, const Vector2& half_extents = Vector2(0.5f, 0.5f), float orientationDegrees = 0.0f);

    virtual ~ColliderPolygon();
	virtual void DebugRender(Renderer& renderer) const noexcept override;
    int GetSides() const;
    void SetSides(int sides);
    const std::vector<Vector2>& GetVerts() const noexcept;
    const Vector2& GetPosition() const;
    virtual void SetPosition(const Vector2& position) noexcept override;
    void Translate(const Vector2& translation);
    void Rotate(float displacementDegrees);
    virtual float GetOrientationDegrees() const noexcept override;
    virtual void SetOrientationDegrees(float degrees) noexcept override;
    const Vector2& GetHalfExtents() const noexcept override;
    void SetHalfExtents(const Vector2& newHalfExtents);
    virtual Vector2 CalcDimensions() const noexcept override;
    virtual float CalcArea() const noexcept override;
    OBB2 GetBounds() const noexcept override;
    virtual Vector2 Support(const Vector2& d) const noexcept override;
    virtual Vector2 CalcCenter() const noexcept override;
protected:
    void CalcNormals();

    void CalcVerts();

	int _sides = 4;
    float _orientationDegrees = 0.0f;
    Vector2 _half_extents = Vector2(0.5f, 0.5f);
    Vector2 _position = Vector2::ZERO;
	std::vector<Vector2> _verts;
	std::vector<Vector2> _normals;
private:
};

class ColliderOBB : public ColliderPolygon {
public:
	ColliderOBB(const Vector2& position, const Vector2& half_extents);
    virtual float CalcArea() const noexcept override;

    virtual void DebugRender(Renderer& renderer) const noexcept override;
    virtual const Vector2& GetHalfExtents() const noexcept override;
    virtual Vector2 Support(const Vector2& d) const noexcept override;
    virtual void SetPosition(const Vector2& position) noexcept override;
    virtual float GetOrientationDegrees() const noexcept override;
    virtual void SetOrientationDegrees(float degrees) noexcept override;
    virtual Vector2 CalcDimensions() const noexcept override;
    virtual OBB2 GetBounds() const noexcept override;
    virtual Vector2 CalcCenter() const noexcept override;

protected:
private:
};

class ColliderCircle : public ColliderPolygon {
public:
    ColliderCircle(const Vector2& position, float radius);
    virtual float CalcArea() const noexcept override;
    virtual const Vector2& GetHalfExtents() const noexcept override;
    virtual Vector2 Support(const Vector2& d) const noexcept override;
    virtual void DebugRender(Renderer& renderer) const noexcept override;
    virtual void SetPosition(const Vector2& position) noexcept override;
    virtual float GetOrientationDegrees() const noexcept override;
    virtual void SetOrientationDegrees(float degrees) noexcept override;
    virtual Vector2 CalcDimensions() const noexcept override;
    virtual OBB2 GetBounds() const noexcept override;
    virtual Vector2 CalcCenter() const noexcept override;

protected:
private:
};

std::tuple<bool, float, Vector2> GJKDistance(const Collider& a, const Collider& b);

namespace MathUtils {
    Vector2 CalcClosestPoint(const Vector2& p, const Collider& collider);
}

struct PhysicsMaterial {
    float friction = 0.7f;
    float restitution = 0.3f;
    float density = 1.0f;
};

struct RigidBodyDesc {
    PhysicsMaterial physicsMaterial = PhysicsMaterial{};
    Vector2 initialPosition = Vector2::ZERO;
    Vector2 initialVelocity = Vector2::ZERO;
    Vector2 initialAcceleration = Vector2::ZERO;
    std::unique_ptr<Collider> collider = std::make_unique<ColliderOBB>(Vector2::ZERO, Vector2::ONE * 0.5f);
    RigidBodyDesc(const RigidBodyDesc& other) = delete;
    RigidBodyDesc(RigidBodyDesc&& other) = default;
    RigidBodyDesc& operator=(const RigidBodyDesc& other) = delete;
    RigidBodyDesc& operator=(RigidBodyDesc&& other) = default;
};

class RigidBody {
public:
    explicit RigidBody(RigidBodyDesc desc = RigidBodyDesc{});
	Matrix4 transform{};

    void BeginFrame();
    void Update(TimeUtils::FPSeconds deltaSeconds);
    void DebugRender(Renderer& renderer) const;
    void Endframe();

    void EnablePhysics(bool enabled);
    void EnableGravity(bool enabled);
    bool IsPhysicsEnabled() const;
    bool IsGravityEnabled() const;

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

    const Vector2& GetPosition() const;
    Vector2 GetVelocity() const;
    const Vector2& GetAcceleration() const;
    Vector2 CalcDimensions() const;
    float GetOrientationDegrees() const;
    float GetAngularVelocityDegrees() const;
    float GetAngularAccelerationDegrees() const;

    bool IsAwake() const;

    const  Collider* GetCollider() const noexcept;
protected:
private:
    std::unique_ptr<Collider> collider = nullptr;
	RigidBody* parent = nullptr;
	std::vector<RigidBody*> children{};
    PhysicsMaterial phys_material{};
    Vector2 prev_position{};
    Vector2 position{};
    Vector2 acceleration{};
    float inv_mass = 1.0f;
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
	bool enable_physics = true;
	bool enable_gravity = true;
	bool is_awake = true;

    friend class PhysicsSystem;
};

struct CollisionData {
    RigidBody* const a = nullptr;
    RigidBody* const b = nullptr;
    float distance = 0.0f;
    Vector2 normal{};
    CollisionData(RigidBody* const a, RigidBody* const b, float distance, Vector2 normal) : a(a), b(b), distance(distance), normal(normal)
    {}
    bool operator==(const CollisionData& rhs) const noexcept {
        return (this->a == rhs.a && this->b == rhs.b) || (this->b == rhs.a && this->a == rhs.b);
    }
    bool operator!=(const CollisionData& rhs) const noexcept {
        return !(*this == rhs);
    }
};

struct PhysicsSystemDesc {
    AABB2 world_bounds = AABB2(Vector2::ZERO, 500.0f, 500.0f);
	float gravity = 980.665f;
	float world_to_meters = 1000.0f;
	int position_solver_iterations = 1;
	int velocity_solver_iterations = 1;
};

class PhysicsSystem {
public:
    explicit PhysicsSystem(Renderer& renderer, const PhysicsSystemDesc& desc = PhysicsSystemDesc{});
	~PhysicsSystem();

	void Initialize() noexcept;
	void BeginFrame() noexcept;
	void Update(TimeUtils::FPSeconds deltaSeconds) noexcept;
    void Render() const noexcept;
	void EndFrame() noexcept;

    void AddObject(RigidBody* body);
    void AddObjects(std::vector<RigidBody*> bodies);
    void RemoveObject(const RigidBody* body);

    void DebugShowCollision(bool show);
    void DebugShowWorldPartition(bool show);
    void Enable(bool enable);
    void SetGravity(float new_gravity);
    float GetGravity() const noexcept;
    void SetWorldDescription(const PhysicsSystemDesc& new_desc);
    void EnableGravity(bool isGravityEnabled) noexcept;
    void EnablePhysics(bool isGravityEnabled) noexcept;
protected:
private:
	void Update_Worker(TimeUtils::FPSeconds deltaSeconds) noexcept;
    void UpdateBodiesInBounds(TimeUtils::FPSeconds deltaSeconds) noexcept;
    std::vector<RigidBody*> BroadPhaseCollision(const AABB2& query_area) noexcept;
    std::vector<CollisionData> NarrowPhaseCollision(std::vector<RigidBody*>& potential_collisions) noexcept;

    Renderer& _renderer;
    PhysicsSystemDesc _desc{};
    std::thread _update_thread{};
    std::condition_variable _signal{};
	std::atomic_bool _is_running = false;
    std::vector<RigidBody*> _rigidBodies{};
    std::vector<const RigidBody*> _pending_removal{};
    QuadTree<RigidBody> _world_partition{};
    bool _show_colliders = false;
    bool _show_world_partition = false;
};
