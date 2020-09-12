#pragma once

#include "Engine/Core/TimeUtils.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vector2.hpp"

#include "Engine/Physics/PhysicsTypes.hpp"
#include "Engine/Physics/RigidBody.hpp"

#include "Engine/Profiling/ProfileLogScope.hpp"

#include "Engine/Renderer/Renderer.hpp"

#include <atomic>
#include <condition_variable>
#include <queue>
#include <set>
#include <thread>
#include <utility>
#include <vector>

struct PhysicsSystemDesc {
    AABB2 world_bounds = AABB2(Vector2::ZERO, 500.0f, 500.0f);
    float gravity = 10.0f;
    Vector2 dragK1K2 = Vector2{1.0f, 1.0f};
    float world_to_meters = 100.0f;
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
    void DebugShowContacts(bool show);

    void Enable(bool enable);
    void SetGravity(float new_gravity);
    float GetGravity() const noexcept;
    void SetDragCoefficients(const Vector2& k1k2);
    void SetDragCoefficients(float linearCoefficient, float squareCoefficient);
    std::pair<float,float> GetDragCoefficients() const noexcept;
    const PhysicsSystemDesc& GetWorldDescription() const noexcept;
    void SetWorldDescription(const PhysicsSystemDesc& new_desc);
    void EnableGravity(bool isGravityEnabled) noexcept;
    void EnableDrag(bool isDragEnabled) noexcept;
    void EnablePhysics(bool isPhysicsEnabled) noexcept;

protected:
private:
    void Update_Worker() noexcept;
    void UpdateBodiesInBounds(TimeUtils::FPSeconds deltaSeconds) noexcept;
    std::vector<RigidBody*> BroadPhaseCollision(const AABB2& query_area) noexcept;

    using CollisionDataSet = std::set<CollisionData, std::equal_to<CollisionData>>;

    template<typename CollisionDetectionFunction, typename CollisionResolutionFunction>
    CollisionDataSet NarrowPhaseCollision(const std::vector<RigidBody*>& potential_collisions, CollisionDetectionFunction&& cd, CollisionResolutionFunction&& cr) noexcept {
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

    void SolveCollision(const CollisionDataSet& actual_collisions) noexcept;

    Renderer& _renderer;
    PhysicsSystemDesc _desc{};
    std::thread _update_thread{};
    std::condition_variable _signal{};
    std::mutex _cs{};
    std::atomic_bool _is_running = false;
    std::atomic_bool _delta_seconds_changed = false;
    std::vector<RigidBody*> _rigidBodies{};
    std::deque<CollisionData> _contacts{};
    std::vector<const RigidBody*> _pending_removal{};
    //QuadTree<RigidBody> _world_partition{};
    std::atomic<float> _deltaSeconds = 0.0f;
    bool _show_colliders = false;
    bool _show_object_bounds = false;
    bool _show_world_partition = false;
    bool _show_contacts = false;
};
