#pragma once

#include <type_traits>

template<typename E>
using is_scoped_enum = std::integral_constant<bool, std::is_enum<E>::value && !std::is_convertible<E, int>::value>;

template<typename E>
constexpr bool is_scoped_enum_v = is_scoped_enum<E>::value;

template<typename E>
struct is_bitflag_enum_type : std::false_type {};

template<typename E>
constexpr bool is_bitflag_enum_type_v = is_bitflag_enum_type<E>::value;

template<typename E>
using is_bitflag_enum = std::integral_constant<bool, is_scoped_enum_v<E> && is_bitflag_enum_type_v<E>>;

template<typename E>
constexpr bool is_bitflag_enum_v = is_bitflag_enum<E>::value;

template<typename E, typename = std::enable_if_t<is_bitflag_enum_v<E>>>
E& operator|=(E& a, const E& b) noexcept {
    using underlying = std::underlying_type_t<E>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    a = static_cast<E>(underlying_a | underlying_b);
    return a;
}

template<typename E, typename = std::enable_if_t<is_bitflag_enum_v<E>>>
E operator|(E a, const E& b) noexcept {
    a |= b;
    return a;
}

template<typename E, typename = std::enable_if_t<is_bitflag_enum_v<E>>>
E& operator&=(E& a, const E& b) noexcept {
    using underlying = std::underlying_type_t<E>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    a = static_cast<E>(underlying_a & underlying_b);
    return a;
}

template<typename E, typename = std::enable_if_t<is_bitflag_enum_v<E>>>
E operator&(E a, const E& b) noexcept {
    a &= b;
    return a;
}

template<typename E, typename = std::enable_if_t<is_bitflag_enum_v<E>>>
E& operator^=(E& a, const E& b) noexcept {
    using underlying = std::underlying_type_t<E>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    a = static_cast<E>(underlying_a ^ underlying_b);
    return a;
}

template<typename E, typename = std::enable_if_t<is_bitflag_enum_v<E>>>
E operator^(E a, const E& b) noexcept {
    a ^= b;
    return a;
}

template<typename E, typename = std::enable_if_t<is_bitflag_enum_v<E>>>
E operator~(E a) noexcept {
    using underlying = std::underlying_type_t<E>;
    auto underlying_a = static_cast<underlying>(a);
    a = static_cast<E>(~underlying_a);
    return a;
}
