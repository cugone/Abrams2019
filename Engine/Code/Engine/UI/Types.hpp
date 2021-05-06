#pragma once

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vector2.hpp"

namespace a2de {

    namespace UI {

        enum class FillMode {
            Fill,
            Fit,
            Stretch,
            Tile,
            Center,
            Span,
            Crop = Fill,
        };

        enum class PivotPosition {
            First_,
            TopLeft = First_,
            Top,
            TopRight,
            Left,
            Center,
            Right,
            BottomLeft,
            Bottom,
            BottomRight,
            Last_,
        };

        PivotPosition& operator++(PivotPosition& mode);
        PivotPosition operator++(PivotPosition& mode, int);

        PivotPosition& operator--(PivotPosition& mode);
        PivotPosition operator--(PivotPosition& mode, int);

        enum class InvalidateElementReason : uint8_t {
            None = 0,
            Layout = 1 << 0,
            Order = 1 << 1,
            Any = Layout | Order,
        };

        InvalidateElementReason operator|(InvalidateElementReason a, const InvalidateElementReason& b);
        InvalidateElementReason& operator|=(InvalidateElementReason& a, const InvalidateElementReason& b);
        InvalidateElementReason operator&(InvalidateElementReason a, const InvalidateElementReason& b);
        InvalidateElementReason& operator&=(InvalidateElementReason& a, const InvalidateElementReason& b);

    } // namespace UI
} // namespace a2de
