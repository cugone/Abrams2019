#pragma once

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vector2.hpp"

namespace UI {
// clang-format off
enum class FillMode {
    Fill
    ,Fit
    ,Stretch
    ,Tile
    ,Center
    ,Span
    ,Crop = Fill
};

enum class PivotPosition {
    First_
    ,TopLeft = First_
    ,Top
    ,TopRight
    ,Left
    ,Center
    ,Right
    ,BottomLeft
    ,Bottom
    ,BottomRight
    ,Last_
};
// clang-format on
PivotPosition& operator++(PivotPosition& mode);
PivotPosition operator++(PivotPosition& mode, int);

PivotPosition& operator--(PivotPosition& mode);
PivotPosition operator--(PivotPosition& mode, int);

// clang-format off
enum class InvalidateElementReason : uint8_t {
    None = 0
    ,Layout = 1 << 0
    ,Order = 1 << 1
    ,Any = Layout | Order
};
// clang-format on
InvalidateElementReason operator|(InvalidateElementReason a, const InvalidateElementReason& b);
InvalidateElementReason& operator|=(InvalidateElementReason& a, const InvalidateElementReason& b);
InvalidateElementReason operator&(InvalidateElementReason a, const InvalidateElementReason& b);
InvalidateElementReason& operator&=(InvalidateElementReason& a, const InvalidateElementReason& b);

} // namespace UI
