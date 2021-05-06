#include "Engine/Math/Ray2.hpp"

namespace a2de {

    void Ray2::SetOrientationDegrees(float angleDegrees) {
        SetOrientationRadians(MathUtils::ConvertDegreesToRadians(angleDegrees));
    }

    void Ray2::SetOrientationRadians(float angleRadians) {
        direction.x = std::cos(angleRadians);
        direction.y = std::sin(angleRadians);
    }

    Vector2 Ray2::Interpolate(float t) {
        return position + t * direction;
    }

} // namespace a2de
