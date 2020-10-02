#include "Engine/Physics/PhysicsTypes.hpp"

CollisionData::CollisionData(RigidBody* const a, RigidBody* const b, float distance, const Vector3& normal)
: a(a)
, b(b)
, distance(distance)
, normal(normal)
{}

bool CollisionData::operator==(const CollisionData& rhs) const noexcept {
    //Strict weak ordering required by std::set
    return (!(this->a < rhs.a) && !(rhs.a < this->a)) && (!(this->b < rhs.b) && !(rhs.b < this->b));
}
bool CollisionData::operator!=(const CollisionData& rhs) const noexcept {
    return !(*this == rhs);
}

Position::Position(Vector2 value) noexcept
: m_value{value}
{

    /* DO NOTHING */
}

Position::Position(float x, float y) noexcept
: m_value{x, y} {
    /* DO NOTHING */
}

Position::operator Vector2 const () {
    return m_value;
}

Position::operator Vector2() {
    return m_value;
}

Vector2 Position::Get() const noexcept {
    return m_value;
}

void Position::Set(Vector2 value) noexcept {
    m_value = value;
}

Velocity::Velocity(Vector2 value) noexcept
: m_value{value}
{
    /* DO NOTHING */
}

Velocity::Velocity(float x, float y) noexcept
: m_value{x, y}
{
    /* DO NOTHING */
}

Velocity::operator Vector2 const() {
    return m_value;
}

Velocity::operator Vector2() {
    return m_value;
}

Vector2 Velocity::Get() const noexcept {
    return m_value;
}

void Velocity::Set(Vector2 value) noexcept {
    m_value = value;
}

Acceleration::Acceleration(Vector2 value) noexcept
: m_value{value}
{
    /* DO NOTHING */
}

Acceleration::Acceleration(float x, float y) noexcept
: m_value{x, y}
{
    /* DO NOTHING */
}

Acceleration::operator Vector2 const() {
    return m_value;
}

Acceleration::operator Vector2() {
    return m_value;
}

Vector2 Acceleration::Get() const noexcept {
    return m_value;
}

void Acceleration::Set(Vector2 value) noexcept {
    m_value = value;
}
