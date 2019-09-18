#include "Engine/Physics/PhysicsSystem.hpp"

void PhysicsSystem::Update_Worker(TimeUtils::FPSeconds /*deltaSeconds*/) noexcept {

}

void PhysicsSystem::Enable(bool enable) {
    _is_running = enable;
}

PhysicsSystem::PhysicsSystem(Renderer& renderer, const PhysicsSystemDesc& desc /*= PhysicsSystemDesc{}*/)
	: _renderer(renderer)
	, _desc(desc)
{
	/* DO NOTHING */
}

PhysicsSystem::~PhysicsSystem() {
	/* DO NOTHING */
}

void PhysicsSystem::Initialize() noexcept {
	/* DO NOTHING */
}

void PhysicsSystem::BeginFrame() noexcept {
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
    if(!this->_is_running) {
        return;
    }
    for(auto& body : _rigidBodies) {
        body->Update(deltaSeconds);
    }
}

void PhysicsSystem::Render() const noexcept {
    if(_show_colliders) {
        for(const auto& body : _rigidBodies) {
            body->DebugRender(_renderer);
        }
    }
}

void PhysicsSystem::EndFrame() noexcept {
    for(auto& body : _rigidBodies) {
        body->Endframe();
    }
    for(const auto* r : _pending_removal) {
        _rigidBodies.erase(std::remove_if(std::begin(_rigidBodies), std::end(_rigidBodies), [this, r](const RigidBody* b) { return b == r; }), std::end(_rigidBodies));
    }
    _pending_removal.clear();
    _pending_removal.shrink_to_fit();
}

void PhysicsSystem::AddObject(RigidBody& body) {
    _rigidBodies.push_back(&body);
}

void PhysicsSystem::RemoveObject(const RigidBody& body) {
    _pending_removal.push_back(&body);
}

void PhysicsSystem::DebugShowCollision(bool show) {
    _show_colliders = show;
}

RigidBody::RigidBody(const RigidBodyDesc desc /*= RigidBodyDesc{}*/)
    : collider(desc.collider)
    , phys_material(desc.physicsMaterial)
    , prev_position(desc.initialPosition)
    , position(desc.initialPosition)
    , acceleration(desc.initialAcceleration)
    , inv_mass(1.0f / (desc.physicsMaterial.density * desc.collider.CalcDimensions().x * desc.collider.CalcDimensions().y))
{
    /* DO NOTHING */
}

void RigidBody::BeginFrame() {
    /* DO NOTHING */
}

void RigidBody::Update(TimeUtils::FPSeconds deltaSeconds) {
    if(!is_awake || !enable_physics || MathUtils::IsEquivalentToZero(inv_mass)) {
        return;
    }
    const auto linear_impulse_sum = std::accumulate(std::begin(linear_impulses), std::end(linear_impulses), Vector2::ZERO);
    const auto angular_impulse_sum = std::accumulate(std::begin(angular_impulses), std::end(angular_impulses), 0.0f);
    linear_impulses.clear();
    angular_impulses.clear();
    const auto linear_force_sum = std::accumulate(std::begin(linear_forces), std::end(linear_forces), Vector2::ZERO);
    const auto angular_force_sum = std::accumulate(std::begin(angular_forces), std::end(angular_forces), 0.0f);
    const auto new_angular_acceleration = angular_impulse_sum  + angular_force_sum * inv_mass;
    const auto new_acceleration = (linear_impulse_sum + linear_force_sum) * inv_mass;
    const auto t = deltaSeconds.count();
    dt = deltaSeconds;
    if(MathUtils::IsEquivalentToZero(prev_position - position)
        && MathUtils::IsEquivalentToZero(prev_orientationDegrees - orientationDegrees)) {
        time_since_last_move += dt;
    } else {
        time_since_last_move = TimeUtils::FPSeconds{ 0.0f };
    }
    is_awake = time_since_last_move < TimeUtils::FPSeconds{ 1.0f };
    const auto new_position = 2.0f * position - prev_position + new_acceleration * t * t;
    const auto new_orientationDegrees = 2.0f * orientationDegrees - prev_orientationDegrees + new_angular_acceleration * t * t;

    prev_position = position;
    position = new_position;
    prev_orientationDegrees = orientationDegrees;
    orientationDegrees = new_orientationDegrees;
    acceleration = new_acceleration;

    const auto S = Matrix4::CreateScaleMatrix(collider.half_extents);
    const auto R = Matrix4::Create2DRotationDegreesMatrix(orientationDegrees);
    const auto T = Matrix4::CreateTranslationMatrix(position);
    const auto M = T * R * S;
    auto new_transform = Matrix4::I;
    if(!parent) {
        new_transform = M;
    } else {
        auto p = parent;
        while(p) {
            new_transform = M * p->GetParentTransform();
            p = p->parent;
        }
    }
    transform = new_transform;
    collider.position = position;
    collider.orientationDegrees = orientationDegrees;
}

void RigidBody::DebugRender(Renderer& renderer) const {
    renderer.SetModelMatrix(Matrix4::I);
    renderer.DrawOBB2(collider, Rgba::Pink);
}

void RigidBody::Endframe() {
    /* DO NOTHING */
}

void RigidBody::EnablePhysics(bool enabled) {
    enable_physics = enabled;
}

void RigidBody::EnableGravity(bool enabled) {
    enable_gravity = enabled;
}

bool RigidBody::IsPhysicsEnabled() const {
    return enable_physics;
}

bool RigidBody::IsGravityEnabled() const {
    return enable_gravity;
}

float RigidBody::GetMass() const {
    return 1.0f / inv_mass;
}

float RigidBody::GetInverseMass() const {
    return inv_mass;
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
    const auto point_of_collision = MathUtils::CalcClosestPoint(position_on_object, collider);
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
    const auto point_of_collision = MathUtils::CalcClosestPoint(position_on_object, collider);
    const auto r = position - point_of_collision;
    const auto [parallel, perpendicular] = MathUtils::DivideIntoProjectAndReject(force, r);
    const auto angular_result = force - parallel;
    const auto linear_result = force - perpendicular;
    ApplyTorque(angular_result.CalcLength());
    ApplyForce(linear_result);
}

void RigidBody::ApplyImpulseAt(const Vector2& position_on_object, const Vector2& direction, float magnitude) {
    ApplyImpulseAt(position_on_object, direction.GetNormalize() * magnitude);
}

void RigidBody::ApplyImpulseAt(const Vector2& position_on_object, const Vector2& force) {
    const auto point_of_collision = MathUtils::CalcClosestPoint(position_on_object, collider);
    const auto r = position - point_of_collision;
    const auto [parallel, perpendicular] = MathUtils::DivideIntoProjectAndReject(force, r);
    const auto angular_result = force - parallel;
    const auto linear_result = force - perpendicular;
    ApplyTorque(angular_result.CalcLength(), true);
    ApplyImpulse(linear_result);
}

const OBB2& RigidBody::GetCollider() const {
    return collider;
}

const Vector2& RigidBody::GetPosition() const {
    return position;
}

Vector2 RigidBody::GetVelocity() const {
    return (position - prev_position) / dt.count();
}

const Vector2& RigidBody::GetAcceleration() const {
    return acceleration;
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

bool RigidBody::IsAwake() const {
    return is_awake;
}
