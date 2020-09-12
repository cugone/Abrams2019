#include "Engine/Physics/PhysicsTypes.hpp"

CollisionData::CollisionData(RigidBody* const a, RigidBody* const b, float distance, const Vector3& normal)
: a(a)
, b(b)
, distance(distance)
, normal(normal)
{}

bool CollisionData::operator==(const CollisionData& rhs) const noexcept {
    return (!(this->a < rhs.a) && !(rhs.a < this->a)) && (!(this->b < rhs.b) && !(rhs.b < this->b));
}
bool CollisionData::operator!=(const CollisionData& rhs) const noexcept {
    return !(*this == rhs);
}
