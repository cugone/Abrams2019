#include "Engine/Physics/PhysicsSystem.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Core/ThreadUtils.hpp"

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
            const auto actual_collisions = NarrowPhaseCollision(potential_collisions);
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

void PhysicsSystem::SetWorldDescription(const PhysicsSystemDesc& new_desc) {
    _desc = new_desc;
    _world_partition.SetWorldBounds(_desc.world_bounds);
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

PhysicsSystem::PhysicsSystem(Renderer& renderer, const PhysicsSystemDesc& desc /*= PhysicsSystemDesc{}*/)
: _renderer(renderer)
, _desc(desc)
, _world_partition(_desc.world_bounds) {
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
    _is_running = true;
    _update_thread = std::thread(&PhysicsSystem::Update_Worker, this);
    ThreadUtils::SetThreadDescription(_update_thread, "Physics Async Update");
}

void PhysicsSystem::BeginFrame() noexcept {
    PROFILE_LOG_SCOPE_FUNCTION();
    if(!_is_running) {
        return;
    }
    for(auto& body : _rigidBodies) {
        if(body->IsGravityEnabled()) {
            body->ApplyForce(Vector2::Y_AXIS * _desc.gravity);
        }
        body->BeginFrame();
    }
}

void PhysicsSystem::Update(TimeUtils::FPSeconds deltaSeconds) noexcept {
    PROFILE_LOG_SCOPE_FUNCTION();
    if(!this->_is_running) {
        return;
    }
    _deltaSeconds = deltaSeconds.count();
    _delta_seconds_changed = true;
    _signal.notify_all();
    //UpdateBodiesInBounds(deltaSeconds);
    //const auto camera_position = Vector2(_renderer.GetCamera().GetPosition());
    //const auto half_extents = Vector2(_renderer.GetOutput()->GetDimensions()) * 0.5f;
    //const auto query_area = AABB2(camera_position - half_extents, camera_position + half_extents);
    //const auto potential_collisions = BroadPhaseCollision(query_area);
    //const auto actual_collisions = NarrowPhaseCollision(potential_collisions);
}

void PhysicsSystem::UpdateBodiesInBounds(TimeUtils::FPSeconds deltaSeconds) noexcept {
    PROFILE_LOG_SCOPE_FUNCTION();
    for(auto body : _rigidBodies) {
        if(!body) {
            continue;
        }
        if(MathUtils::DoOBBsOverlap(OBB2(_desc.world_bounds), body->GetBounds())) {
            body->Update(deltaSeconds);
        }
    }
}

std::vector<RigidBody*> PhysicsSystem::BroadPhaseCollision(const AABB2& query_area) noexcept {
    PROFILE_LOG_SCOPE_FUNCTION();
    std::vector<RigidBody*> potential_collisions{};
    for(auto body : _rigidBodies) {
        if(!body) {
            continue;
        }
        if(!MathUtils::DoOBBsOverlap(OBB2(query_area), body->GetBounds())) {
            continue;
        }
        if(MathUtils::DoOBBsOverlap(OBB2(_desc.world_bounds), body->GetBounds())) {
            const auto queried_bodies = _world_partition.Query(query_area);
            for(auto* query : queried_bodies) {
                potential_collisions.push_back(query);
            }
        }
    }
    return potential_collisions;
}

std::set<CollisionData, std::equal_to<CollisionData>> PhysicsSystem::NarrowPhaseCollision(const std::vector<RigidBody*>& potential_collisions) noexcept {
    PROFILE_LOG_SCOPE_FUNCTION();
    std::set<CollisionData, std::equal_to<CollisionData>> result{};
    if(potential_collisions.size() < 2) {
        return {};
    }
    for(auto iter_a = potential_collisions.begin(); iter_a != potential_collisions.end() - 1; ++iter_a) {
        for(auto iter_b = iter_a + 1; iter_b != potential_collisions.end(); ++iter_b) {
            auto* const cur_body = *iter_a;
            auto* const next_body = *iter_b;
            const auto collisionResult = GJKDistance(*cur_body->GetCollider(), *next_body->GetCollider());
            if(collisionResult.collides) {
                const auto [where_inserted, was_inserted] = result.insert(CollisionData{cur_body, next_body, collisionResult.distance, collisionResult.collisionNormal});
                if(was_inserted) {
                    DebuggerPrintf("Physics System: Attempting to insert already existing element.");
                }
            }
        }
    }
    return result;
}

void PhysicsSystem::Render() const noexcept {
    if(_show_colliders) {
        for(const auto& body : _rigidBodies) {
            body->DebugRender(_renderer);
        }
    }
    if(_show_world_partition) {
        _world_partition.DebugRender(_renderer);
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
    _world_partition.Clear();
    _world_partition.Add(_rigidBodies);
    _signal.notify_all();
}

void PhysicsSystem::AddObject(RigidBody* body) {
    _rigidBodies.push_back(body);
    _world_partition.Add(body);
}

void PhysicsSystem::AddObjects(std::vector<RigidBody*> bodies) {
    _rigidBodies.reserve(_rigidBodies.size() + bodies.size());
    for(auto* body : bodies) {
        AddObject(body);
    }
}

void PhysicsSystem::RemoveObject(const RigidBody* body) {
    _pending_removal.push_back(body);
}

void PhysicsSystem::DebugShowCollision(bool show) {
    _show_colliders = show;
}

void PhysicsSystem::DebugShowWorldPartition(bool show) {
    _show_world_partition = show;
}

RigidBody::RigidBody(RigidBodyDesc desc /*= RigidBodyDesc{}*/)
: collider(std::move(desc.collider))
, phys_material(desc.physicsMaterial)
, prev_position(desc.initialPosition)
, position(desc.initialPosition)
, acceleration(desc.initialAcceleration)
, inv_mass(1.0f) {
    const auto area = collider->CalcArea();
    inv_mass = (1.0f / (desc.physicsMaterial.density * area));
}

RigidBody::RigidBody([[maybe_unused]] const PhysicsSystemDesc& physicsDesc, RigidBodyDesc&& desc /*= RigidBodyDesc{}*/)
: rigidbodyDesc{std::move(desc)}
, prev_position(rigidbodyDesc.initialPosition)
, position(rigidbodyDesc.initialPosition)
, acceleration(rigidbodyDesc.initialAcceleration)
{
    const auto area = rigidbodyDesc.collider->CalcArea();
    if(/*MathUtils::IsEquivalentToZero(rigidbodyDesc.physicsMaterial.density) ||*/ MathUtils::IsEquivalentToZero(area)) {
        rigidbodyDesc.physicsDesc.mass = 0.0f;
    } else {
        //rigidbodyDesc.physicsDesc.mass = /*rigidbodyDesc.physicsMaterial.density **/ area / physicsDesc.world_to_meters;
        //rigidbodyDesc.physicsDesc.mass = std::pow(rigidbodyDesc.physicsDesc.mass, rigidbodyDesc.physicsMaterial.massExponent);
        if(!MathUtils::IsEquivalentToZero(rigidbodyDesc.physicsDesc.mass) && rigidbodyDesc.physicsDesc.mass < 0.001f) {
            rigidbodyDesc.physicsDesc.mass = 0.001f;
        }
    }
}

void RigidBody::BeginFrame() {
    /* DO NOTHING */
}

void RigidBody::Update(TimeUtils::FPSeconds deltaSeconds) {
    PROFILE_LOG_SCOPE_FUNCTION();
    if(!is_awake || !IsPhysicsEnabled() || MathUtils::IsEquivalentToZero(GetInverseMass())) {
        linear_impulses.clear();
        angular_impulses.clear();
        return;
    }
    const auto inv_mass = GetInverseMass();
    const auto linear_impulse_sum = std::accumulate(std::begin(linear_impulses), std::end(linear_impulses), Vector2::ZERO);
    const auto angular_impulse_sum = std::accumulate(std::begin(angular_impulses), std::end(angular_impulses), 0.0f);
    linear_impulses.clear();
    angular_impulses.clear();
    const auto linear_force_sum = std::accumulate(std::begin(linear_forces), std::end(linear_forces), Vector2::ZERO);
    const auto angular_force_sum = std::accumulate(std::begin(angular_forces), std::end(angular_forces), 0.0f);
    const auto new_angular_acceleration = angular_impulse_sum + angular_force_sum * inv_mass;
    const auto new_acceleration = (linear_impulse_sum + linear_force_sum) * inv_mass;
    const auto t = deltaSeconds.count();
    dt = deltaSeconds;
    if(MathUtils::IsEquivalentToZero(prev_position - position)
       && MathUtils::IsEquivalentToZero(prev_orientationDegrees - orientationDegrees)) {
        time_since_last_move += dt;
    } else {
        time_since_last_move = TimeUtils::FPSeconds{0.0f};
    }
    is_awake = time_since_last_move < TimeUtils::FPSeconds{1.0f};
    const auto new_position = 2.0f * position - prev_position + new_acceleration * t * t;
    const auto new_orientationDegrees = 2.0f * orientationDegrees - prev_orientationDegrees + new_angular_acceleration * t * t;

    prev_position = position;
    position = new_position;
    prev_orientationDegrees = orientationDegrees;
    orientationDegrees = new_orientationDegrees;
    acceleration = new_acceleration;

    const auto collider = GetCollider();
    const auto S = Matrix4::CreateScaleMatrix(collider->GetHalfExtents());
    const auto R = Matrix4::Create2DRotationDegreesMatrix(orientationDegrees);
    const auto T = Matrix4::CreateTranslationMatrix(position);
    const auto M = Matrix4::MakeSRT(S, R, T);
    auto new_transform = Matrix4::I;
    if(!parent) {
        new_transform = M;
    } else {
        auto p = parent;
        while(p) {
            new_transform = Matrix4::MakeRT(p->GetParentTransform(), M);
            p = p->parent;
        }
    }
    transform = new_transform;
    collider->SetPosition(position);
    collider->SetOrientationDegrees(orientationDegrees);
}

void RigidBody::DebugRender(Renderer& renderer) const {
    renderer.SetModelMatrix(Matrix4::I);
    rigidbodyDesc.collider->DebugRender(renderer);
    renderer.DrawOBB2(GetBounds(), Rgba::Green);
}

void RigidBody::Endframe() {
    /* DO NOTHING */
}

void RigidBody::EnablePhysics(bool enabled) {
    rigidbodyDesc.physicsDesc.enablePhysics = enabled;
}

void RigidBody::EnableGravity(bool enabled) {
    rigidbodyDesc.physicsDesc.enableGravity = enabled;
}

void RigidBody::EnableDrag(bool enabled) {
    rigidbodyDesc.physicsDesc.enableDrag = enabled;
}

bool RigidBody::IsPhysicsEnabled() const {
    return rigidbodyDesc.physicsDesc.enablePhysics;
}

bool RigidBody::IsGravityEnabled() const {
    return rigidbodyDesc.physicsDesc.enableGravity;
}

bool RigidBody::IsDragEnabled() const {
    return rigidbodyDesc.physicsDesc.enableDrag;
}

void RigidBody::SetAwake(bool awake) noexcept {
    is_awake = awake;
}

void RigidBody::Wake() noexcept {
    SetAwake(true);
}

void RigidBody::Sleep() noexcept {
    SetAwake(false);
}

bool RigidBody::IsAwake() const {
    return is_awake;
}

float RigidBody::GetMass() const {
    return rigidbodyDesc.physicsDesc.mass;
}

float RigidBody::GetInverseMass() const {
    if(const auto mass = GetMass(); mass > 0.0f) {
        return 1.0f / mass;
    }
    return 0.0f;
}

Matrix4 RigidBody::GetParentTransform() const {
    if(parent) {
        return parent->transform;
    }
    return Matrix4::I;
}

void RigidBody::ApplyImpulse(const Vector2& impulse) {
    is_awake = true;
    linear_impulses.push_back(impulse);
}

void RigidBody::ApplyImpulse(const Vector2& direction, float magnitude) {
    ApplyImpulse(direction.GetNormalize() * magnitude);
}

void RigidBody::ApplyForce(const Vector2& force) {
    is_awake = true;
    linear_forces.push_back(force);
}

void RigidBody::ApplyForce(const Vector2& direction, float magnitude) {
    ApplyForce(direction.GetNormalize() * magnitude);
}

void RigidBody::ApplyTorque(float force, bool asImpulse /*= false*/) {
    is_awake = true;
    if(asImpulse) {
        angular_impulses.push_back(force);
    } else {
        angular_forces.push_back(force);
    }
}

void RigidBody::ApplyTorqueAt(const Vector2& position_on_object, const Vector2& direction, float magnitude, bool asImpulse /*= false*/) {
    ApplyTorqueAt(position_on_object, direction.GetNormalize() * magnitude, asImpulse);
}

void RigidBody::ApplyTorqueAt(const Vector2& position_on_object, const Vector2& force, bool asImpulse /*= false*/) {
    const auto collider = GetCollider();
    const auto point_of_collision = MathUtils::CalcClosestPoint(position_on_object, *collider);
    const auto r = position - point_of_collision;
    const auto torque = MathUtils::CrossProduct(force, r);
    ApplyTorque(torque, asImpulse);
}

void RigidBody::ApplyTorque(const Vector2& direction, float magnitude, bool asImpulse /*= false*/) {
    ApplyTorqueAt(position, direction * magnitude, asImpulse);
}

void RigidBody::ApplyForceAt(const Vector2& position_on_object, const Vector2& direction, float magnitude) {
    ApplyForceAt(position_on_object, direction.GetNormalize() * magnitude);
}

void RigidBody::ApplyForceAt(const Vector2& position_on_object, const Vector2& force) {
    const auto collider = GetCollider();
    const auto point_of_collision = MathUtils::CalcClosestPoint(position_on_object, *collider);
    auto r = position - point_of_collision;
    if(MathUtils::IsEquivalentToZero(r)) {
        r = position;
    }
    const auto [parallel, perpendicular] = MathUtils::DivideIntoProjectAndReject(force, r);
    const auto angular_result = force - parallel;
    const auto linear_result = force - perpendicular;
    ApplyTorqueAt(position_on_object, angular_result);
    ApplyForce(linear_result);
}

void RigidBody::ApplyImpulseAt(const Vector2& position_on_object, const Vector2& direction, float magnitude) {
    ApplyImpulseAt(position_on_object, direction.GetNormalize() * magnitude);
}

void RigidBody::ApplyImpulseAt(const Vector2& position_on_object, const Vector2& force) {
    const auto collider = GetCollider();
    const auto point_of_collision = MathUtils::CalcClosestPoint(position_on_object, *collider);
    const auto r = position - point_of_collision;
    const auto [parallel, perpendicular] = MathUtils::DivideIntoProjectAndReject(force, r);
    const auto angular_result = force - parallel;
    const auto linear_result = force - perpendicular;
    ApplyTorqueAt(position_on_object, angular_result.GetNormalize(), angular_result.CalcLength(), true);
    ApplyImpulse(linear_result);
}

const OBB2 RigidBody::GetBounds() const {
    const auto center = GetPosition();
    const auto dims = CalcDimensions();
    const auto orientation = GetOrientationDegrees();
    return OBB2(center, dims * 0.5f, orientation);
}

void RigidBody::SetPosition(const Vector2& newPosition, bool teleport /*= false*/) noexcept {
    if(teleport) {
        position = newPosition;
        prev_position = newPosition;
    } else {
        Wake();
        position = newPosition;
    }
}

const Vector2& RigidBody::GetPosition() const {
    return position;
}

Vector2 RigidBody::GetVelocity() const {
    if(dt.count()) {
        return (position - prev_position) / dt.count();
    }
    return Vector2::ZERO;
}

const Vector2& RigidBody::GetAcceleration() const {
    return acceleration;
}

Vector2 RigidBody::CalcDimensions() const {
    return GetCollider()->CalcDimensions();
}

float RigidBody::GetOrientationDegrees() const {
    return orientationDegrees;
}

float RigidBody::GetAngularVelocityDegrees() const {
    return (orientationDegrees - prev_orientationDegrees) / dt.count();
}

float RigidBody::GetAngularAccelerationDegrees() const {
    return angular_acceleration;
}

const Collider* RigidBody::GetCollider() const noexcept {
    return rigidbodyDesc.collider.get();
}

Collider* RigidBody::GetCollider() noexcept {
    return rigidbodyDesc.collider.get();
}

ColliderPolygon::ColliderPolygon(int sides /*= 4*/, const Vector2& position /*= Vector2::ZERO*/, const Vector2& half_extents /*= Vector2(0.5f, 0.5f)*/, float orientationDegrees /*= 0.0f*/)
: Collider()
, _polygon{sides, position, half_extents, orientationDegrees}
{
    /* DO NOTHING */
}

ColliderPolygon::ColliderPolygon()
: Collider()
{
    /* DO NOTHING */
}

ColliderPolygon::~ColliderPolygon() = default;

void ColliderPolygon::DebugRender(Renderer& renderer) const noexcept {
    renderer.DrawPolygon2D(_polygon, Rgba::White);
}

int ColliderPolygon::GetSides() const {
    return _polygon.GetSides();
}

void ColliderPolygon::SetSides(int sides) {
    _polygon.SetSides(sides);
}

const std::vector<Vector2>& ColliderPolygon::GetVerts() const noexcept {
    return _polygon.GetVerts();
}

const Vector2& ColliderPolygon::GetPosition() const {
    return _polygon.GetPosition();
}

void ColliderPolygon::SetPosition(const Vector2& position) noexcept {
    _polygon.SetPosition(position);
}

void ColliderPolygon::Translate(const Vector2& translation) {
    _polygon.Translate(translation);
}

void ColliderPolygon::RotateDegrees(float displacementDegrees) {
    _polygon.RotateDegrees(displacementDegrees);
}

void ColliderPolygon::Rotate(float displacementRadians) {
    _polygon.Rotate(displacementRadians);
}

float ColliderPolygon::GetOrientationDegrees() const noexcept {
    return _polygon.GetOrientationDegrees();
}

void ColliderPolygon::SetOrientationDegrees(float degrees) noexcept {
    _polygon.SetOrientationDegrees(degrees);
}

const Vector2& ColliderPolygon::GetHalfExtents() const noexcept {
    return _polygon.GetHalfExtents();
}

void ColliderPolygon::SetHalfExtents(const Vector2& newHalfExtents) {
    _polygon.SetHalfExtents(newHalfExtents);
}

Vector2 ColliderPolygon::CalcDimensions() const noexcept {
    const auto verts = _polygon.GetVerts();
    const auto [min_x, max_x] = std::minmax_element(std::cbegin(verts), std::cend(verts), [](const Vector2& a, const Vector2& b) {
        return a.x < b.x;
    });
    const auto [min_y, max_y] = std::minmax_element(std::cbegin(verts), std::cend(verts), [](const Vector2& a, const Vector2& b) {
        return a.y < b.y;
    });
    const float width = (*max_x).x - (*min_x).x;
    const float height = (*max_y).y - (*min_y).y;
    return Vector2{width, height};
}

float ColliderPolygon::CalcArea() const noexcept {
    float A = 0.0f;
    const auto verts = _polygon.GetVerts();
    auto s = verts.size();
    for(std::size_t i = 0; i < s - 1; ++i) {
        A = verts[i].x * verts[i + 1].y - verts[i + 1].x * verts[i].y;
    }
    return 0.5f * A;
}

OBB2 ColliderPolygon::GetBounds() const noexcept {
    return OBB2(_polygon.GetPosition(), CalcDimensions() * 0.5f, _polygon.GetOrientationDegrees());
}

Vector2 ColliderPolygon::Support(const Vector2& d) const noexcept {
    const auto& verts = _polygon.GetVerts();
    return *std::max_element(std::cbegin(verts), std::cend(verts), [&d](const Vector2& a, const Vector2& b) { return MathUtils::DotProduct(a, d.GetNormalize()) < MathUtils::DotProduct(b, d.GetNormalize()); });
}

Vector2 ColliderPolygon::CalcCenter() const noexcept {
    return _polygon.GetPosition();
}

const Polygon2& ColliderPolygon::GetPolygon() const noexcept {
    return _polygon;
}

ColliderOBB::ColliderOBB(const Vector2& position, const Vector2& half_extents)
: ColliderPolygon(4, position, half_extents, 0.0f) {
    /* DO NOTHING */
}

float ColliderOBB::CalcArea() const noexcept {
    const auto dims = CalcDimensions();
    return dims.x * dims.y;
}

void ColliderOBB::DebugRender(Renderer& renderer) const noexcept {
    renderer.DrawOBB2(_polygon.GetOrientationDegrees(), Rgba::Pink);
}

const Vector2& ColliderOBB::GetHalfExtents() const noexcept {
    return _polygon.GetHalfExtents();
}

Vector2 ColliderOBB::Support(const Vector2& d) const noexcept {
    return ColliderPolygon::Support(d);
}

void ColliderOBB::SetPosition(const Vector2& position) noexcept {
    ColliderPolygon::SetPosition(position);
}

float ColliderOBB::GetOrientationDegrees() const noexcept {
    return _polygon.GetOrientationDegrees();
}

void ColliderOBB::SetOrientationDegrees(float degrees) noexcept {
    ColliderPolygon::SetOrientationDegrees(degrees);
}

Vector2 ColliderOBB::CalcDimensions() const noexcept {
    return _polygon.GetHalfExtents() * 2.0f;
}

OBB2 ColliderOBB::GetBounds() const noexcept {
    return OBB2(_polygon.GetPosition(), _polygon.GetHalfExtents(), _polygon.GetOrientationDegrees());
}

Vector2 ColliderOBB::CalcCenter() const noexcept {
    return _polygon.GetPosition();
}

ColliderCircle::ColliderCircle(const Vector2& position, float radius)
: ColliderPolygon(16, position, Vector2(radius, radius), 0.0f) {
    /* DO NOTHING */
}

float ColliderCircle::CalcArea() const noexcept {
    const auto half_extents = _polygon.GetHalfExtents();
    return MathUtils::M_PI * half_extents.x * half_extents.x;
}

const Vector2& ColliderCircle::GetHalfExtents() const noexcept {
    return _polygon.GetHalfExtents();
}

Vector2 ColliderCircle::Support(const Vector2& d) const noexcept {
    return _polygon.GetPosition() + d.GetNormalize() * _polygon.GetHalfExtents().x;
    //return ColliderPolygon::Support(d);
}

void ColliderCircle::DebugRender(Renderer& renderer) const noexcept {
    renderer.DrawCircle2D(_polygon.GetPosition(), _polygon.GetHalfExtents().x, Rgba::Pink);
}

void ColliderCircle::SetPosition(const Vector2& position) noexcept {
    ColliderPolygon::SetPosition(position);
}

float ColliderCircle::GetOrientationDegrees() const noexcept {
    return _polygon.GetOrientationDegrees();
}

void ColliderCircle::SetOrientationDegrees(float degrees) noexcept {
    return ColliderPolygon::SetOrientationDegrees(degrees);
}

Vector2 ColliderCircle::CalcDimensions() const noexcept {
    return _polygon.GetHalfExtents() * 2.0f;
}

OBB2 ColliderCircle::GetBounds() const noexcept {
    return OBB2(_polygon.GetPosition(), _polygon.GetHalfExtents(), _polygon.GetOrientationDegrees());
}

Vector2 ColliderCircle::CalcCenter() const noexcept {
    return _polygon.GetPosition();
}

Vector2 MathUtils::CalcClosestPoint(const Vector2& p, const Collider& collider) {
    return collider.Support(p - collider.CalcCenter());
}

bool GJKIntersect(const Collider& a, const Collider& b) {
    const auto calcMinkowskiDiff = [](const Vector2& direction, const Collider& a) { return a.Support(direction.GetNormalize()); };
    const auto support = [&](const Vector2& direction) { return calcMinkowskiDiff(direction, a) - calcMinkowskiDiff(-direction, b); };
    auto A = support(Vector2::X_AXIS);
    std::vector<Vector2> simplex{A};
    auto D = -A;
    bool containsOrigin = false;
    const auto doSimplexLine = [&](std::vector<Vector2>& simplex, Vector2& D) {
        const auto i_b = simplex.size() - 2;
        const auto i_a = simplex.size() - 1;
        const auto pointA = simplex[i_a];
        const auto pointB = simplex[i_b];
        const auto lineAB = pointB - pointA;
        const auto lineAO = -pointA;
        if(MathUtils::DotProduct(lineAB, lineAO) > 0.0f) {
            D.SetHeadingDegrees(90.0f - 90.0f * MathUtils::DotProduct(lineAB, lineAO));
            simplex.clear();
            simplex.push_back(pointA);
            simplex.push_back(pointB);
        } else {
            D = lineAO.GetNormalize();
            simplex.clear();
            simplex.push_back(A);
        }
    };
    const auto doSimplexTriangle = [&](std::vector<Vector2>& simplex, Vector2& D) {
        const auto i_c = simplex.size() - 3;
        const auto i_b = simplex.size() - 2;
        const auto i_a = simplex.size() - 1;
        const auto pointC = simplex[i_c];
        const auto pointB = simplex[i_b];
        const auto pointA = simplex[i_a];
        const auto lineAC = pointC - pointA;
        const auto lineAB = pointB - pointA;
        const auto lineAO = -pointA;
        if(MathUtils::DotProduct(lineAC, lineAO) > 0.0f) {
            D.SetHeadingDegrees(90.0f - 90.0f * MathUtils::DotProduct(lineAC, lineAO));
            simplex.clear();
            simplex.push_back(pointA);
            simplex.push_back(pointC);
            containsOrigin = false;
        } else if(MathUtils::DotProduct(lineAB, lineAO) > 0.0f) {
            D.SetHeadingDegrees(90.0f - 90.0f * MathUtils::DotProduct(lineAB, lineAO));
            simplex.clear();
            simplex.push_back(pointA);
            simplex.push_back(pointB);
            containsOrigin = false;
        } else {
            containsOrigin = true;
        }
    };
    const auto doSimplex = [&](std::vector<Vector2>& simplex, Vector2& D) {
        const auto S = simplex.size();
        switch(S) {
        case 2:
            doSimplexLine(simplex, D);
            break;
        case 3:
            doSimplexTriangle(simplex, D);
            break;
        default:
            break;
        }
    };
    const auto result = [&](std::vector<Vector2>& simplex, Vector2& D) {
        for(;;) {
            A = support(D);
            if(MathUtils::DotProduct(A, D) < 0.0f) {
                return false;
            }
            simplex.push_back(A);
            doSimplex(simplex, D);
            if(containsOrigin) {
                return true;
            }
        }
    }(simplex, D); //IIIL
    return result;
}

Vector2 GJKClosestPoint(const Collider& a, const Collider& b) {
    const auto calcMinkowskiDiff = [](const Vector2& direction, const Collider& a) { return a.Support(direction); };
    const auto support = [&](const Vector2& direction) { return calcMinkowskiDiff(direction, a) - calcMinkowskiDiff(-direction, b); };
    auto A = support((b.CalcCenter() - a.CalcCenter()).GetNormalize());
    std::vector<Vector2> simplex{A};
    auto D = -A;
    bool containsOrigin = false;
    const auto doSimplexLine = [&](std::vector<Vector2>& simplex, Vector2& D) {
        const auto i_b = simplex.size() - 2;
        const auto i_a = simplex.size() - 1;
        const auto pointA = simplex[i_a];
        const auto pointB = simplex[i_b];
        const auto lineAB = pointB - pointA;
        const auto lineAO = -pointA;
        if(MathUtils::DotProduct(lineAB, lineAO) > 0.0f) {
            D.SetHeadingDegrees(90.0f - 90.0f * MathUtils::DotProduct(lineAB, lineAO));
            simplex.clear();
            simplex.push_back(pointA);
            simplex.push_back(pointB);
        } else {
            D = lineAO.GetNormalize();
            simplex.clear();
            simplex.push_back(A);
        }
    };
    const auto doSimplexTriangle = [&](std::vector<Vector2>& simplex, Vector2& D) {
        const auto i_c = simplex.size() - 3;
        const auto i_b = simplex.size() - 2;
        const auto i_a = simplex.size() - 1;
        const auto pointC = simplex[i_c];
        const auto pointB = simplex[i_b];
        const auto pointA = simplex[i_a];
        const auto lineAC = pointC - pointA;
        const auto lineAB = pointB - pointA;
        const auto lineAO = -pointA;
        if(MathUtils::DotProduct(lineAC, lineAO) > 0.0f) {
            D.SetHeadingDegrees(90.0f - 90.0f * MathUtils::DotProduct(lineAC, lineAO));
            simplex.clear();
            simplex.push_back(pointA);
            simplex.push_back(pointC);
            containsOrigin = false;
        } else if(MathUtils::DotProduct(lineAB, lineAO) > 0.0f) {
            D.SetHeadingDegrees(90.0f - 90.0f * MathUtils::DotProduct(lineAB, lineAO));
            simplex.clear();
            simplex.push_back(pointA);
            simplex.push_back(pointB);
            containsOrigin = false;
        } else {
            containsOrigin = true;
        }
    };
    const auto doSimplex = [&](std::vector<Vector2>& simplex, Vector2& D) {
        const auto S = simplex.size();
        switch(S) {
        case 1:
            ERROR_AND_DIE("doSimplex reached degenerate case of 1.");
            break;
        case 2:
            doSimplexLine(simplex, D);
            break;
        case 3:
            doSimplexTriangle(simplex, D);
            break;
        case 4:
            ERROR_AND_DIE("doSimplex reached complex case of 4.");
            break;
        default:
            ERROR_AND_DIE("doSimplex reached default case.");
            break;
        }
    };
    const auto result = [&](std::vector<Vector2>& simplex, Vector2& D) {
        for(;;) {
            A = support(D);
            if(MathUtils::DotProduct(A, D) < 0.0f) {
                return false;
            }
            simplex.push_back(A);
            doSimplex(simplex, D);
            if(containsOrigin) {
                return true;
            }
        }
    }(simplex, D); //IIIL
    return simplex.back();
}

GJKResult GJKDistance(const Collider& a, const Collider& b) {
    const auto calcMinkowskiDiff = [](const Vector2& direction, const Collider& a) { return a.Support(direction); };
    const auto support = [&](const Vector2& direction) { return calcMinkowskiDiff(direction, a) - calcMinkowskiDiff(-direction, b); };
    auto A = support((b.CalcCenter() - a.CalcCenter()).GetNormalize());
    std::vector<Vector2> simplex{A};
    auto D = -A;
    bool containsOrigin = false;
    const auto doSimplexLine = [&](std::vector<Vector2>& simplex, Vector2& D) {
        const auto i_b = simplex.size() - 2;
        const auto i_a = simplex.size() - 1;
        const auto pointA = simplex[i_a];
        const auto pointB = simplex[i_b];
        const auto lineAB = pointB - pointA;
        const auto lineAO = (-pointA).GetNormalize();
        if(MathUtils::DotProduct(lineAB, lineAO) > 0.0f) {
            D.SetHeadingDegrees(90.0f - 90.0f * MathUtils::DotProduct(lineAB, lineAO));
            simplex.clear();
            simplex.push_back(pointA);
            simplex.push_back(pointB);
        } else {
            D = lineAO.GetNormalize();
            simplex.clear();
            simplex.push_back(A);
        }
    };
    const auto doSimplexTriangle = [&](std::vector<Vector2>& simplex, Vector2& D) {
        const auto i_c = simplex.size() - 3;
        const auto i_b = simplex.size() - 2;
        const auto i_a = simplex.size() - 1;
        const auto pointC = simplex[i_c];
        const auto pointB = simplex[i_b];
        const auto pointA = simplex[i_a];
        const auto lineAC = pointC - pointA;
        const auto lineAB = pointB - pointA;
        const auto lineAO = (-pointA).GetNormalize();
        if(MathUtils::DotProduct(lineAC, lineAO) > 0.0f) {
            D.SetHeadingDegrees(90.0f - 90.0f * MathUtils::DotProduct(lineAC, lineAO));
            simplex.clear();
            simplex.push_back(pointA);
            simplex.push_back(pointC);
            containsOrigin = false;
        } else if(MathUtils::DotProduct(lineAB, lineAO) > 0.0f) {
            D.SetHeadingDegrees(90.0f - 90.0f * MathUtils::DotProduct(lineAB, lineAO));
            simplex.clear();
            simplex.push_back(pointA);
            simplex.push_back(pointB);
            containsOrigin = false;
        } else {
            containsOrigin = true;
        }
    };
    const auto doSimplex = [&](std::vector<Vector2>& simplex, Vector2& D) {
        const auto S = simplex.size();
        switch(S) {
        case 2:
            doSimplexLine(simplex, D);
            break;
        case 3:
            doSimplexTriangle(simplex, D);
            break;
        default:
            break;
        }
    };
    const auto result = [&](std::vector<Vector2>& simplex, Vector2& D) {
        for(;;) {
            const auto simplex_copy = simplex;
            A = support(D);
            if(MathUtils::DotProduct(A, D) < 0.0f) {
                return false;
            }
            simplex.push_back(A);
            doSimplex(simplex, D);
            if(simplex_copy == simplex) {
                return false;
            }
            if(containsOrigin) {
                return true;
            }
        }
    }(simplex, D); //IIIL
    return GJKResult{result, A.CalcLength(), D};
}
