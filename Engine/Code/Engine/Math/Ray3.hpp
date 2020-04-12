#pragma once

#include "Engine/Math/Vector3.hpp"

class Ray3 {
public:
    Vector3 position;
    Vector3 direction;

    Vector3 Interpolate(float t);

protected:
private:
    
};
