#include "Engine/Physics/Collider.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Physics/PhysicsTypes.hpp"

ColliderPolygon::ColliderPolygon(int sides /*= 4*/, const Vector2& position /*= Vector2::ZERO*/, const Vector2& half_extents /*= Vector2(0.5f, 0.5f)*/, float orientationDegrees /*= 0.0f*/)
: Collider()
, _polygon{sides, position, half_extents, orientationDegrees} {
    /* DO NOTHING */
}

ColliderPolygon::ColliderPolygon()
: Collider() {
    /* DO NOTHING */
}

void ColliderPolygon::DebugRender(Renderer& renderer) const noexcept {
    renderer.DrawPolygon2D(Vector2::ZERO, 0.5f, _polygon.GetSides(), Rgba::Pink);
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
    for(std::size_t i = 0; i < s; ++i) {
        std::size_t j = (i + 1) % s;
        A += (verts[i].x * verts[j].y) - (verts[j].x * verts[i].y);
    }
    return 0.5f * std::abs(A);
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

ColliderPolygon* ColliderPolygon::Clone() const noexcept {
    return new ColliderPolygon(GetSides(), GetPosition(), GetHalfExtents(), GetOrientationDegrees());
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

ColliderOBB* ColliderOBB::Clone() const noexcept {
    return new ColliderOBB(GetPosition(), GetHalfExtents());
}

ColliderCircle::ColliderCircle(const Position& position, float radius)
: ColliderPolygon(16, position.Get(), Vector2(radius, radius), 0.0f) {
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
    renderer.DrawCircle2D(Vector2::ZERO, 0.5f, Rgba::Pink);// _polygon.GetPosition(), _polygon.GetHalfExtents().x, Rgba::Pink);
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

ColliderCircle* ColliderCircle::Clone() const noexcept {
    return new ColliderCircle(GetPosition(), GetHalfExtents().x);
}
