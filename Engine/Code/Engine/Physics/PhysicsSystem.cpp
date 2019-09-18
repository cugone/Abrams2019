#include "Engine/Physics/PhysicsSystem.hpp"

void PhysicsSystem::Update_Worker(TimeUtils::FPSeconds /*deltaSeconds*/) noexcept {

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
    for(auto& body : _rigidBodies) {
        body->ApplyForce(Vector2::Y_AXIS * _desc.gravity);
        body->BeginFrame();
    }
}

void PhysicsSystem::Update(TimeUtils::FPSeconds deltaSeconds) noexcept {
    if(!this->_is_running) {
        return;
    }
    for(auto& body : _rigidBodies) {
        if(body->enable_physics) {
            body->Update(deltaSeconds);
        }
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
}

void PhysicsSystem::AddObject(RigidBody& body) {
    _rigidBodies.push_back(&body);
}

void PhysicsSystem::DebugShowCollision(bool show) {
    _show_colliders = show;
}

RigidBody::RigidBody(const RigidBodyDesc desc /*= RigidBodyDesc{}*/)
    : collider(desc.collider)
    , phys_material(desc.physicsMaterial)
    , prev_position(desc.initialPosition)
    , position(desc.initialPosition)
    , velocity(desc.initialVelocity)
    , acceleration(desc.initialAcceleration)
    , inv_mass(1.0f / (desc.physicsMaterial.density * desc.collider.CalcDimensions().x * desc.collider.CalcDimensions().y))
{
    /* DO NOTHING */
}

void RigidBody::BeginFrame() {
    /* DO NOTHING */
}

void RigidBody::Update(TimeUtils::FPSeconds deltaSeconds) {
    if(!enable_physics || MathUtils::IsEquivalentToZero(inv_mass)) {
        return;
    }
    const auto linear_impulse_sum = std::accumulate(std::begin(linear_impulses), std::end(linear_impulses), Vector2::ZERO);
    const auto linear_force_sum = std::accumulate(std::begin(linear_forces), std::end(linear_forces), Vector2::ZERO);
    const auto angular_force_sum = std::accumulate(std::begin(angular_forces), std::end(angular_forces), Vector2::ZERO);
    const auto new_angular_acceleration = angular_force_sum * inv_mass;
    const auto new_acceleration = (linear_impulse_sum + linear_force_sum) * inv_mass;
    const auto t = deltaSeconds.count();
    const auto new_position = 2.0f * position - prev_position + new_acceleration * t * t;
    //TODO: Fix for ACTUAL angular acceleration
    const auto new_orientationDegrees = 2.0f * orientationDegrees - prev_orientationDegrees + new_angular_acceleration.CalcLength() * t * t;

    prev_position = position;
    position = new_position;
    velocity = (position - prev_position) / t;
    prev_orientationDegrees = orientationDegrees;
    orientationDegrees = new_orientationDegrees;
    angular_velocity = (orientationDegrees - prev_orientationDegrees) / t;
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
    linear_impulses.clear();
    angular_impulses.clear();
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
    linear_impulses.push_back(impulse);
}

void RigidBody::ApplyImpulse(const Vector2& direction, float magnitude) {
    ApplyImpulse(direction.GetNormalize() * magnitude);
}

void RigidBody::ApplyForce(const Vector2& force) {
    linear_forces.push_back(force);
}

void RigidBody::ApplyForce(const Vector2& direction, float magnitude) {
    ApplyForce(direction.GetNormalize() * magnitude);
}

void RigidBody::ApplyTorque(const Vector2& force, bool asImpulse /*= false*/) {
    if(asImpulse) {
        angular_impulses.push_back(force);
    } else {
        angular_forces.push_back(force);
    }
}

void RigidBody::ApplyTorque(const Vector2& direction, float magnitude, bool asImpulse /*= false*/) {
    ApplyTorque(direction.GetNormalize() * magnitude, asImpulse);
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
    ApplyTorque(angular_result * r.CalcLength());
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
    ApplyTorque(angular_result * r.CalcLength(), true);
    ApplyImpulse(linear_result);
}
