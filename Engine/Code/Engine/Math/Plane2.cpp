#include "Engine/Math/Plane2.hpp"

Plane2::Plane2(const Vector2& normal, float distance_from_origin)
    : normal(normal.GetNormalize())
    , dist(distance_from_origin)
{
    /* DO NOTHING */
}

float Plane2::Normalize() {
    float length = normal.CalcLength();
    if(length > 0.0f) {
        float inv_length = 1.0f / length;
        normal.x *= inv_length;
        normal.y *= inv_length;
        dist *= inv_length;
    }
    return length;
}

Plane2 Plane2::GetNormalize() const {
    Plane2 result(*this);
    result.Normalize();
    return result;
}
