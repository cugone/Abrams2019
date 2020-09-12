#pragma once

#include "Engine/Physics/PhysicsTypes.hpp"

class Collider;

namespace PhysicsUtils {
    GJKResult GJK(const Collider& a, const Collider& b);
    bool GJKIntersect(const Collider& a, const Collider& b);

    EPAResult EPA(const GJKResult& gjk, const Collider& a, const Collider& b);
    bool SAT(const Collider& a, const Collider& b);
}

namespace MathUtils {
    Vector2 CalcClosestPoint(const Vector2& p, const Collider& collider);
}
