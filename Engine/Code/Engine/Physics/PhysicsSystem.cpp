#include "Engine/Physics/PhysicsSystem.hpp"

#include "Engine/Physics/PhysicsUtils.hpp"

#include "Engine/Math/Plane2.hpp"

#include <algorithm>
#include <mutex>

//TODO: Multi-threaded update when things get hairy.
void PhysicsSystem::Update_Worker() noexcept {
    while(_is_running) {
        std::unique_lock<std::mutex> lock(_cs);
        //Condition to wake up: Not running or deltaSeconds has changed.
        _signal.wait(lock, [this]() -> bool { return !_is_running || _delta_seconds_changed; });
        if(_delta_seconds_changed) {
            _delta_seconds_changed = false;
            UpdateBodiesInBounds(TimeUtils::FPSeconds(_deltaSeconds));
            const auto camera_position = Vector2(_renderer.GetCamera().GetPosition());
            const auto half_extents = Vector2(_renderer.GetOutput()->GetDimensions()) * 0.5f;
            const auto query_area = AABB2(camera_position - half_extents, camera_position + half_extents);
            const auto potential_collisions = BroadPhaseCollision(query_area);
            const auto actual_collisions = NarrowPhaseCollision(potential_collisions, PhysicsUtils::GJK, PhysicsUtils::EPA);
            SolveCollision(actual_collisions);
        }
    }
}

void PhysicsSystem::Enable(bool enable) {
    _is_running = enable;
}

void PhysicsSystem::SetGravity(float new_gravity) {
    _desc.gravity = new_gravity;
}

float PhysicsSystem::GetGravity() const noexcept {
    return _desc.gravity;
}

void PhysicsSystem::SetDragCoefficients(const Vector2& k1k2) {
    _desc.dragK1K2 = k1k2;
}

void PhysicsSystem::SetDragCoefficients(float linearCoefficient, float squareCoefficient) {
    SetDragCoefficients(Vector2{linearCoefficient, squareCoefficient});
}

std::pair<float, float> PhysicsSystem::GetDragCoefficients() const noexcept {
    return std::make_pair(_desc.dragK1K2.x, _desc.dragK1K2.y);
}

const PhysicsSystemDesc& PhysicsSystem::GetWorldDescription() const noexcept {
    return _desc;
}

void PhysicsSystem::SetWorldDescription(const PhysicsSystemDesc& new_desc) {
    _desc = new_desc;
    //_world_partition.SetWorldBounds(_desc.world_bounds);
}

void PhysicsSystem::EnablePhysics(bool isPhysicsEnabled) noexcept {
    for(auto& b : _rigidBodies) {
        b->EnablePhysics(isPhysicsEnabled);
    }
}

void PhysicsSystem::EnableGravity(bool isGravityEnabled) noexcept {
    for(auto& b : _rigidBodies) {
        b->EnableGravity(isGravityEnabled);
    }
}

void PhysicsSystem::EnableDrag(bool isGravityEnabled) noexcept {
    for(auto& b : _rigidBodies) {
        b->EnableDrag(isGravityEnabled);
    }
}

PhysicsSystem::PhysicsSystem(Renderer& renderer, const PhysicsSystemDesc& desc /*= PhysicsSystemDesc{}*/)
: _renderer(renderer)
, _desc(desc)
//, _world_partition(_desc.world_bounds)
{
    /* DO NOTHING */
}

PhysicsSystem::~PhysicsSystem() {
    _is_running = false;
    _signal.notify_all();
    if(_update_thread.joinable()) {
        _update_thread.join();
    }
}

void PhysicsSystem::Initialize() noexcept {
    //_is_running = true;
    //_update_thread = std::thread(&PhysicsSystem::Update_Worker, this);
    //ThreadUtils::SetThreadDescription(_update_thread, "Physics Async Update");
}

void PhysicsSystem::BeginFrame() noexcept {
    PROFILE_LOG_SCOPE_FUNCTION();
    if(!_is_running) {
        return;
    }

    //_rigidBodies.reserve(_rigidBodies.size() + _pending_addition.size());
    for(auto* a : _pending_addition) {
        _rigidBodies.emplace_back(a);
    }
    _pending_addition.clear();
    _pending_addition.shrink_to_fit();
    //_world_partition.Clear();
    //_world_partition.Add(_rigidBodies);

    for(auto& body : _rigidBodies) {
        //TODO: Refactor to gravity and drag Force Generators
        if(body->IsGravityEnabled()) {
            body->ApplyForce(Vector2::Y_AXIS * _desc.gravity);
        }
        if(body->IsDragEnabled()) {
            auto dragForce = body->GetVelocity();
            auto dragCoeff = dragForce.CalcLength();
            const auto [k1, k2] = GetDragCoefficients();
            dragCoeff = k1 * dragCoeff + k2 * dragCoeff * dragCoeff;
            dragForce.Normalize();
            dragForce *= -dragCoeff;
            body->ApplyForce(dragForce);
        }
        body->BeginFrame();
    }
}

void PhysicsSystem::Update(TimeUtils::FPSeconds deltaSeconds) noexcept {
    PROFILE_LOG_SCOPE_FUNCTION();
    if(!this->_is_running) {
        //_signal.notify_all();
        return;
    }
    _deltaSeconds = deltaSeconds.count();
    //_delta_seconds_changed = true;
    //_signal.notify_all();
    UpdateBodiesInBounds(TimeUtils::FPSeconds(_deltaSeconds));
    const auto camera_position = Vector2(_renderer.GetCamera().GetPosition());
    const auto half_extents = Vector2(_renderer.GetOutput()->GetDimensions()) * 0.5f;
    const auto query_area = AABB2(camera_position - half_extents, camera_position + half_extents);
    const auto potential_collisions = BroadPhaseCollision(query_area);
    const auto actual_collisions = NarrowPhaseCollision(potential_collisions, PhysicsUtils::GJK, PhysicsUtils::EPA);
    SolveCollision(actual_collisions);
}

void PhysicsSystem::UpdateBodiesInBounds(TimeUtils::FPSeconds deltaSeconds) noexcept {
    PROFILE_LOG_SCOPE_FUNCTION();
    for(auto body : _rigidBodies) {
        if(!body) {
            continue;
        }
        body->Update(deltaSeconds);
        //if(!MathUtils::DoOBBsOverlap(OBB2(_desc.world_bounds), body->GetBounds())) {
        //    body->FellOutOfWorld();
        //}
        if(MathUtils::IsPointInFrontOfPlane(body->GetPosition(), Plane2(Vector2::Y_AXIS, _desc.kill_plane_distance))) {
            body->FellOutOfWorld();
        }
    }
}

std::vector<RigidBody*> PhysicsSystem::BroadPhaseCollision(const AABB2& /*query_area*/) noexcept {
    PROFILE_LOG_SCOPE_FUNCTION();
    std::vector<RigidBody*> potential_collisions{};
    for(auto iterA = std::begin(_rigidBodies); iterA != std::end(_rigidBodies); ++iterA) {
        for(auto iterB = iterA + 1; iterB != std::end(_rigidBodies); ++iterB) {
            const auto bodyA = *iterA;
            const auto bodyB = *iterB;
            if(!bodyA || !bodyB) {
                continue;
            }
            const auto bodyABoundsAsAABB2 = AABB2{bodyA->GetBounds()};
            const auto bodyBBoundsAsAABB2 = AABB2{bodyB->GetBounds()};
            //if(!MathUtils::DoAABBsOverlap(query_area, bodyBoundsAsAABB2)) {
            //    continue;
            //}
            if(MathUtils::DoAABBsOverlap(bodyABoundsAsAABB2, bodyBBoundsAsAABB2)) {
                //const auto queried_bodies = _world_partition.Query(query_area);
                //for(auto* query : queried_bodies) {
                    //potential_collisions.push_back(query);
                //}
                potential_collisions.push_back(bodyA);
                potential_collisions.push_back(bodyB);
            }
        }
    }
    return potential_collisions;
}

void PhysicsSystem::SolveCollision(const PhysicsSystem::CollisionDataSet& actual_collisions) noexcept {
    const auto maxVSolves = _desc.velocity_solver_iterations;
    const auto maxPSolves = _desc.position_solver_iterations;
    for(auto& collision : actual_collisions) {
        auto* a = collision.a;
        auto* b = collision.b;
        const auto& aPos = a->GetPosition();
        const auto& bPos = b->GetPosition();
        const auto& aMass = a->GetMass();
        const auto& bMass = b->GetMass();
        const auto massSum = aMass + bMass;
        const auto aNormal = Vector2{collision.normal};
        const auto bNormal = Vector2{-collision.normal};
        const auto aDeltaContribution = (aMass / massSum);
        const auto bDeltaContribution = (bMass / massSum);
        const auto aForceContribution = aNormal * aDeltaContribution;
        const auto bForceContribution = bNormal * bDeltaContribution;
        const auto aDistanceContribution = collision.distance * aDeltaContribution;
        const auto bDistanceContribution = collision.distance * bDeltaContribution;
        a->SetPosition(aPos + aDistanceContribution * aNormal);
        b->SetPosition(bPos + bDistanceContribution * bNormal);
        collision.a->ApplyForceAt(aPos, aForceContribution);
        collision.b->ApplyForceAt(bPos, bForceContribution);
    }
}

void PhysicsSystem::Render() const noexcept {
    if(_show_colliders) {
        for(const auto& body : _rigidBodies) {
            body->DebugRender(_renderer);
        }
    }
    if(_show_world_partition) {
        //_world_partition.DebugRender(_renderer);
    }
    if(_show_contacts) {
        _renderer.SetModelMatrix(Matrix4::I);
    }
}

void PhysicsSystem::EndFrame() noexcept {
    //std::scoped_lock<std::mutex> lock(_cs);
    for(auto& body : _rigidBodies) {
        body->Endframe();
    }
    for(const auto* r : _pending_removal) {
        _rigidBodies.erase(std::remove_if(std::begin(_rigidBodies), std::end(_rigidBodies), [this, r](const RigidBody* b) { return b == r; }), std::end(_rigidBodies));
    }
    _pending_removal.clear();
    _pending_removal.shrink_to_fit();
    _signal.notify_all();
}

void PhysicsSystem::AddObject(RigidBody* body) {
    _pending_addition.push_back(body);
    //_world_partition.Add(body);
}

void PhysicsSystem::AddObjects(std::vector<RigidBody*> bodies) {
    _pending_addition.reserve(_rigidBodies.size() + bodies.size());
    _pending_addition.insert(std::cend(_pending_addition), std::cbegin(bodies), std::cend(bodies));
}

void PhysicsSystem::RemoveObject(RigidBody* body) {
    _pending_removal.push_back(body);
}

void PhysicsSystem::RemoveObjects(std::vector<RigidBody*> bodies) {
    _pending_removal.insert(std::cend(_pending_removal), std::cbegin(bodies), std::cend(bodies));
}

void PhysicsSystem::RemoveAllObjects() noexcept {
    _pending_removal.insert(std::cend(_pending_removal), std::cbegin(_rigidBodies), std::cend(_rigidBodies));
}

void PhysicsSystem::RemoveAllObjectsImmediately() noexcept {
    _rigidBodies.clear();
    _rigidBodies.shrink_to_fit();
}

void PhysicsSystem::DebugShowCollision(bool show) {
    _show_colliders = show;
}

void PhysicsSystem::DebugShowWorldPartition(bool show) {
    _show_world_partition = show;
}

void PhysicsSystem::DebugShowContacts(bool show) {
    _show_contacts = show;
}
