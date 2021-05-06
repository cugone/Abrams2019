#include "Engine/Math/Ray3.hpp"

namespace a2de {

    void Ray3::SetDirection(const Vector3& newDirection) noexcept {
        direction = newDirection.GetNormalize();
    }

    Vector3 Ray3::Interpolate(float t) {
        return position + t * direction;
    }

} // namespace a2de
