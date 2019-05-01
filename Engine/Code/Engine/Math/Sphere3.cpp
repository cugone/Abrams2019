#include "Engine/Math/Sphere3.hpp"

#include "Engine/Math/MathUtils.hpp"

const Sphere3 Sphere3::UNIT_SPHERE(0.0f, 0.0f, 0.0f, 1.0f);

Sphere3::Sphere3(float initialX, float initialY, float initialZ, float initialRadius)
    : center(initialX, initialY, initialZ)
    , radius(initialRadius)
{
    /* DO NOTHING */
}

Sphere3::Sphere3(const Vector3& initialCenter, float initialRadius)
    : center(initialCenter)
    , radius(initialRadius)
{
    /* DO NOTHING */
}

void Sphere3::StretchToIncludePoint(const Vector3& point) {
    if(MathUtils::CalcDistanceSquared(center, point) < (radius * radius)) {
        return;
    }
    radius = MathUtils::CalcDistance(center, point);
}

void Sphere3::AddPadding(float paddingRadius) {
    radius += paddingRadius;
}

void Sphere3::Translate(const Vector3& translation) {
    center += translation;
}

Sphere3 Sphere3::operator+(const Vector3& translation) const {
    return Sphere3(center + translation, radius);
}

Sphere3 Sphere3::operator-(const Vector3& antiTranslation) const {
    return Sphere3(center - antiTranslation, radius);
}

Sphere3& Sphere3::operator-=(const Vector3& antiTranslation) {
    center -= antiTranslation;
    return *this;
}

Sphere3& Sphere3::operator+=(const Vector3& translation) {
    center += translation;
    return *this;
}
