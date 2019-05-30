#include "Engine/UI/Types.hpp"

#include <algorithm>

namespace UI {

PivotPosition& operator++(PivotPosition& mode) {
    using underlying = std::underlying_type_t<PivotPosition>;
    mode = static_cast<PivotPosition>(static_cast<underlying>(mode) + 1);
    if(mode == PivotPosition::Last_) {
        mode = PivotPosition::First_;
    }
    return mode;
}

PivotPosition operator++(PivotPosition& mode, int) {
    PivotPosition result = mode;
    ++result;
    return result;
}

PivotPosition& operator--(PivotPosition& mode) {
    if(mode == PivotPosition::First_) {
        mode = PivotPosition::Last_;
    }
    using underlying = std::underlying_type_t<PivotPosition>;
    mode = static_cast<PivotPosition>(static_cast<underlying>(mode) - 1);
    return mode;
}

PivotPosition operator--(PivotPosition& mode, int) {
    PivotPosition result = mode;
    --result;
    return result;
}

Ratio::Ratio(const Vector2& newValue /*= Vector2::ZERO*/) {
    SetValue(newValue);
}

Ratio& Ratio::operator=(const Ratio& rhs) {
    SetValue(rhs.value);
    return *this;
}

Ratio& Ratio::operator=(Ratio&& rhs) {
    SetValue(std::move(rhs.value));
    rhs.value = Vector2::ZERO;
    return *this;
}

Ratio::Ratio(Ratio&& rhs) {
    SetValue(std::move(rhs.value));
    rhs.value = Vector2::ZERO;
}

Ratio::Ratio(const Ratio& rhs) {
    SetValue(rhs.value);
}

const Vector2& Ratio::GetValue() const {
    return value;
}

void Ratio::SetValue(const Vector2& newValue) {
    value.x = std::clamp(newValue.x, 0.0f, 1.0f);
    value.y = std::clamp(newValue.y, 0.0f, 1.0f);
}

} //End UI