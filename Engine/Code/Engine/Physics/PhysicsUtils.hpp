#pragma once

#include "Engine/Physics/PhysicsTypes.hpp"

namespace a2de {

    class Collider;

    namespace PhysicsUtils {
        [[nodiscard]] GJKResult GJK(const Collider& a, const Collider& b);
        [[nodiscard]] bool GJKIntersect(const Collider& a, const Collider& b);

        [[nodiscard]] EPAResult EPA(GJKResult gjk, const Collider& a, const Collider& b);
        [[nodiscard]] bool SAT(const Collider& a, const Collider& b);
    }

    namespace MathUtils {
        [[nodiscard]] Vector2 CalcClosestPoint(const Vector2& p, const Collider& collider);
    }

} // namespace a2de
