#include "Engine/Physics/RigidBody.hpp"

#include "Engine/Profiling/ProfileLogScope.hpp"

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Physics/PhysicsSystem.hpp"
#include "Engine/Physics/PhysicsUtils.hpp"

#include "Engine/Renderer/Renderer.hpp"

#include <numeric>

RigidBody::RigidBody(PhysicsSystem* physicsSystem, const RigidBodyDesc& desc /*= RigidBodyDesc{}*/)
: parentPhysicsSystem(physicsSystem)
, rigidbodyDesc(desc)
, prev_position(rigidbodyDesc.initialPosition - rigidbodyDesc.initialVelocity)
, position(rigidbodyDesc.initialPosition)
, acceleration(rigidbodyDesc.initialAcceleration) {
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
        linear_forces.clear();
        angular_forces.clear();
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
    auto deltaPosition = position - prev_position;
    auto deltaOrientation = orientationDegrees - prev_orientationDegrees;
    if(MathUtils::IsEquivalentToZero(deltaPosition)
       && MathUtils::IsEquivalentToZero(deltaOrientation)) {
        time_since_last_move += dt;
    } else {
        time_since_last_move = TimeUtils::FPSeconds{0.0f};
    }
    is_awake = time_since_last_move < TimeUtils::FPSeconds{1.0f};
    //St�rmer method as described on Wikipedia:
    //https://en.wikipedia.org/wiki/Verlet_integration#Verlet_integration_(without_velocities)
    //As of 2020-08-09 and version VS2019 16.7.3 hyper link parsing is broken.
    //The closing parenthesis in the above link is required.
    //Either copy-and-paste or add it back manually when Ctrl+Clicking drops it.
    auto new_linear_velocity = 2.0f * position - prev_position;
    const auto new_position = new_linear_velocity + new_acceleration * t * t;
    const auto& maxAngularSpeed = rigidbodyDesc.physicsDesc.maxAngularSpeed;
    auto new_angular_velocity = std::clamp(2.0f * orientationDegrees - prev_orientationDegrees, -maxAngularSpeed, maxAngularSpeed);

    if(MathUtils::IsEquivalentToZero(new_angular_velocity)) {
        new_angular_velocity = 0.0f;
    }
    const auto new_orientationDegrees = MathUtils::Wrap(new_angular_velocity + new_angular_acceleration * t * t, 0.0f, 360.0f);

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
    return rigidbodyDesc.collider;
}

Collider* RigidBody::GetCollider() noexcept {
    return rigidbodyDesc.collider;
}

}
