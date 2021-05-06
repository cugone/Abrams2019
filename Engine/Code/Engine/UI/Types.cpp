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

InvalidateElementReason operator|(InvalidateElementReason lhs, const InvalidateElementReason& rhs) {
    lhs |= rhs;
    return lhs;
}

InvalidateElementReason& operator|=(InvalidateElementReason& lhs, const InvalidateElementReason& rhs) {
    auto underlying_lhs = std::underlying_type_t<InvalidateElementReason>(lhs);
    auto underlying_rhs = std::underlying_type_t<InvalidateElementReason>(rhs);
    lhs = static_cast<InvalidateElementReason>(underlying_lhs | underlying_rhs);
    return lhs;
}

InvalidateElementReason operator&(InvalidateElementReason lhs, const InvalidateElementReason& rhs) {
    lhs &= rhs;
    return lhs;
}

InvalidateElementReason& operator&=(InvalidateElementReason& lhs, const InvalidateElementReason& rhs) {
    auto underlying_lhs = std::underlying_type_t<InvalidateElementReason>(lhs);
    auto underlying_rhs = std::underlying_type_t<InvalidateElementReason>(rhs);
    lhs = static_cast<InvalidateElementReason>(underlying_lhs & underlying_rhs);
    return lhs;
}

} // namespace UI