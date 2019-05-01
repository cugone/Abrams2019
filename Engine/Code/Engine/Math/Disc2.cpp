#include "Engine/Math/Disc2.hpp"

#include "Engine/Math/MathUtils.hpp"

const Disc2 Disc2::UNIT_CIRCLE(0.0f, 0.0f, 1.0f);

Disc2::Disc2(float initialX, float initialY, float initialRadius)
    : center(initialX, initialY)
    , radius(initialRadius)
{
    /* DO NOTHING */
}

Disc2::Disc2(const Vector2& initialCenter, float initialRadius)
    : center(initialCenter)
    , radius(initialRadius)
{
    /* DO NOTHING */
}

void Disc2::StretchToIncludePoint(const Vector2& point) {
    if(MathUtils::CalcDistanceSquared(center, point) < (radius * radius)) {
        return;
    }
    radius = MathUtils::CalcDistance(center, point);
}

void Disc2::AddPadding(float paddingRadius) {
    radius += paddingRadius;
}

void Disc2::Translate(const Vector2& translation) {
    center += translation;
}

Disc2 Disc2::operator-(const Vector2& antiTranslation) {
    return Disc2(center - antiTranslation, radius);
}

Disc2& Disc2::operator-=(const Vector2& antiTranslation) {
    center -= antiTranslation;
    return *this;
}

Disc2 Disc2::operator+(const Vector2& translation) {
    return Disc2(center + translation, radius);
}

Disc2& Disc2::operator+=(const Vector2& translation) {
    center += translation;
    return *this;
}
