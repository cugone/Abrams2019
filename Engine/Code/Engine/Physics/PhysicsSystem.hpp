#pragma once

#include "Engine/Core/TimeUtils.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vector2.hpp"

#include "Engine/Physics/PhysicsTypes.hpp"
#include "Engine/Physics/RigidBody.hpp"
#include "Engine/Physics/ForceGenerator.hpp"
#include "Engine/Physics/GravityForceGenerator.hpp"
#include "Engine/Physics/DragForceGenerator.hpp"
#include "Engine/Physics/Joint.hpp"
#include "Engine/Physics/SpringJoint.hpp"
#include "Engine/Physics/RodJoint.hpp"
#include "Engine/Physics/CableJoint.hpp"

#include "Engine/Profiling/ProfileLogScope.hpp"

#include "Engine/Renderer/Renderer.hpp"

#include <atomic>
#include <condition_variable>
#include <queue>
#include <set>
#include <thread>
#include <utility>
#include <vector>

class Joint;

struct PhysicsSystemDesc {
    AABB2 world_bounds{Vector2::ZERO, 500.0f, 500.0f};
    Vector2 gravity{0.0f, 10.0f};
    Vector2 dragK1K2{1.0f, 1.0f};
    float world_to_meters{100.0f};
    float kill_plane_distance{10000.0f};
    int position_solver_iterations{6};
    int velocity_solver_iterations{8};
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
    void RemoveObject(RigidBody* body);
    void RemoveObjects(std::vector<RigidBody*> bodies);
    void RemoveAllObjects() noexcept;
    void RemoveAllObjectsImmediately() noexcept;

    void DebugShowCollision(bool show);
    void DebugShowWorldPartition(bool show);
    void DebugShowContacts(bool show);
    void DebugShowJoints(bool show);

    void Enable(bool enable);
    void SetGravity(const Vector2& new_gravity);
    Vector2 GetGravity() const noexcept;
    void SetDragCoefficients(const Vector2& k1k2);
    void SetDragCoefficients(float linearCoefficient, float squareCoefficient);
    std::pair<float,float> GetDragCoefficients() const noexcept;
    const PhysicsSystemDesc& GetWorldDescription() const noexcept;
    void SetWorldDescription(const PhysicsSystemDesc& new_desc);
    void EnableGravity(bool isGravityEnabled) noexcept;
    void EnableDrag(bool isDragEnabled) noexcept;
    void EnablePhysics(bool isPhysicsEnabled) noexcept;

    template<typename JointDefType>
    Joint* CreateJoint(const JointDefType& defType);

    template<typename ForceGeneratorType>
    ForceGeneratorType* CreateForceGenerator();

protected:
private:
    void UpdateBodiesInBounds(TimeUtils::FPSeconds deltaSeconds) noexcept;
    void ApplyCustomAndJointForces(TimeUtils::FPSeconds deltaSeconds) noexcept;
    void ApplyGravityAndDrag(TimeUtils::FPSeconds deltaSeconds) noexcept;
    std::vector<RigidBody*> BroadPhaseCollision(const AABB2& query_area) noexcept;

    using CollisionDataSet = std::set<CollisionData, std::equal_to<CollisionData>>;
    template<typename CollisionDetectionFunction, typename CollisionResolutionFunction>
    CollisionDataSet NarrowPhaseCollision(const std::vector<RigidBody*>& potential_collisions, CollisionDetectionFunction&& cd, CollisionResolutionFunction&& cr) noexcept;

    void SolveCollision(const CollisionDataSet& actual_collisions) noexcept;
    void SolveConstraints() const noexcept;
    void SolvePositionConstraints() const noexcept;
    void SolveVelocityConstraints() const noexcept;

    Renderer& _renderer;
    PhysicsSystemDesc _desc{};
    bool _is_running = false;
    std::vector<RigidBody*> _rigidBodies{};
    std::deque<CollisionData> _contacts{};
    std::vector<RigidBody*> _pending_removal{};
    std::vector<RigidBody*> _pending_addition{};
    GravityForceGenerator _gravityFG{Vector2::ZERO};
    DragForceGenerator _dragFG{Vector2::ZERO};
    std::vector<std::unique_ptr<ForceGenerator>> _forceGenerators{};
    std::vector<std::unique_ptr<Joint>> _joints{};
    //QuadTree<RigidBody> _world_partition{};
    TimeUtils::FPSeconds _deltaSeconds = TimeUtils::FPSeconds::zero();
    bool _show_colliders = false;
    bool _show_object_bounds = false;
    bool _show_world_partition = false;
    bool _show_contacts = false;
    bool _show_joints = false;
};

template<typename JointDefType>
Joint* PhysicsSystem::CreateJoint(const JointDefType& defType) {
    Joint* newJoint{nullptr};
    if constexpr(std::is_same_v<JointDefType, SpringJointDef>) {
        newJoint = new SpringJoint(defType);
    } else if constexpr(std::is_same_v<JointDefType, RodJointDef>) {
        newJoint = new RodJoint(defType);
    } else if constexpr(std::is_same_v<JointDefType, CableJointDef>) {
        newJoint = new CableJoint(defType);
    } else {
        static_assert(true, "CreateJoint received undeclared type.");
    }
    _joints.emplace_back(newJoint);
    return newJoint;
}

template<typename ForceGeneratorType>
ForceGeneratorType* PhysicsSystem::CreateForceGenerator() {
    auto* newFG = new ForceGenerator();
    _forceGenerators.emplace_back(newFG);
    return newFG;
}
template<typename CollisionDetectionFunction, typename CollisionResolutionFunction>
PhysicsSystem::CollisionDataSet PhysicsSystem::NarrowPhaseCollision(const std::vector<RigidBody*>& potential_collisions, CollisionDetectionFunction&& cd, CollisionResolutionFunction&& cr) noexcept {
    PROFILE_LOG_SCOPE_FUNCTION();
    CollisionDataSet result{};
    if(potential_collisions.size() < 2) {
        _contacts.clear();
        return {};
    }
    for(auto iter_a = std::begin(potential_collisions); iter_a != std::end(potential_collisions); ++iter_a) {
        for(auto iter_b = iter_a + 1; iter_b != std::end(potential_collisions); ++iter_b) {
            auto* const cur_body = *iter_a;
            auto* const next_body = *iter_b;
            if(cur_body == next_body) {
                continue;
            }
            const auto cdResult = std::invoke(cd, *cur_body->GetCollider(), *next_body->GetCollider());
            if(cdResult.collides) {
                const auto crResult = std::invoke(cr, cdResult, *cur_body->GetCollider(), *next_body->GetCollider());
                const auto contact = CollisionData{cur_body, next_body, crResult.distance, crResult.normal};
                const auto [_, was_inserted] = result.insert(contact);
                if(!was_inserted) {
                    DebuggerPrintf("Physics System: Attempting to insert already existing element.");
                } else {
                    while(_contacts.size() >= 10) {
                        _contacts.pop_front();
                    }
                    _contacts.push_back(contact);
                }
            }
        }
    }
    return result;
}
