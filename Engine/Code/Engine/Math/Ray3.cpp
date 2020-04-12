#include "Engine/Math/Ray3.hpp"

Vector3 Ray3::Interpolate(float t) {
    return position + t * direction;
}
