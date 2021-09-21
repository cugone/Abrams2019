#include "Engine/Platform/Win.hpp"

#if defined(PLATFORM_WINDOWS)

AABB2 RectToAABB2(const RECT& rect) noexcept {
    return AABB2(static_cast<float>(rect.left),
                 static_cast<float>(rect.top),
                 static_cast<float>(rect.right),
                 static_cast<float>(rect.bottom));
}

RECT AABB2ToRect(const AABB2& aabb2) noexcept {
    return RECT{static_cast<long>(aabb2.mins.x),
                static_cast<long>(aabb2.mins.y),
                static_cast<long>(aabb2.maxs.x),
                static_cast<long>(aabb2.maxs.y)};
}

#endif
