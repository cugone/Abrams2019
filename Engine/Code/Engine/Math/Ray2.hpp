#pragma once

#include "Engine/Math/Vector2.hpp"

#include "Engine/Math/MathUtils.hpp"

class Ray2 {
public:
    Vector2 position;
    Vector2 direction;

    void SetOrientationDegrees(float angleDegrees);
    void SetOrientationRadians(float angleRadians);
    Vector2 Interpolate(float t);

protected:
private:
};
