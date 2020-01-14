#include "Engine/UI/Types.hpp"

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
    ++mode;
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
    --mode;
    return result;
}

Ratio::Ratio(const Vector2& newValue /*= Vector2::ZERO*/) {
    SetValue(newValue);
}

Ratio& Ratio::operator=(const Ratio& rhs) {
    SetValue(rhs.value);
    return *this;
}

Ratio& Ratio::operator=(Ratio&& rhs) noexcept {
    SetValue(std::move(rhs.value));
    rhs.value = Vector2::ZERO;
    return *this;
}

Ratio::Ratio(Ratio&& rhs) noexcept {
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
    auto clamped_newValue = newValue;
    clamped_newValue.x = std::clamp(clamped_newValue.x, 0.0f, 1.0f);
    clamped_newValue.y = std::clamp(clamped_newValue.y, 0.0f, 1.0f);
    value = clamped_newValue;
}

} //End UI