#pragma once

#include "Engine/Math/Vector2.hpp"

#include "Engine/Math/MathUtils.hpp"

class Ray2 {
public:
    Vector2 position{};
    Vector2 direction{1.0f, 0.0f};

    void SetOrientationDegrees(float angleDegrees);
    void SetOrientationRadians(float angleRadians);
    [[nodiscard]] Vector2 Interpolate(float t);

protected:
private:
};
