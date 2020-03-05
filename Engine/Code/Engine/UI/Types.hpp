#pragma once

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vector2.hpp"

namespace UI {

class Ratio {
public:
    explicit Ratio(const Vector2& newValue = Vector2::ZERO);
    Ratio(const Ratio& rhs);
    Ratio(Ratio&& rhs) noexcept;
    Ratio& operator=(const Ratio& rhs);
    Ratio& operator=(Ratio&& rhs) noexcept;
    ~Ratio() = default;
    const Vector2& GetValue() const;
    void SetValue(const Vector2& newValue);

private:
    Vector2 value{};
};

struct Metric {
    Ratio ratio{};
    Vector2 unit{};
};

enum class PositionMode {
    Absolute,
    Relative,
};

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

} // namespace UI