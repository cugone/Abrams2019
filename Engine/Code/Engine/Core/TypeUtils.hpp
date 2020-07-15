#pragma once

#include <type_traits>

namespace TypeUtils {

    template<typename E>
    using is_scoped_enum = std::integral_constant<bool, std::is_enum<E>::value && !std::is_convertible<E, int>::value>;

    template<typename E>
    constexpr bool is_scoped_enum_v = is_scoped_enum<E>::value;

    template<typename E>
    struct is_bitflag_enum_type : std::false_type {};

    template<typename E>
    constexpr bool is_bitflag_enum_type_v = is_bitflag_enum_type<E>::value;

    template<typename E>
    using is_bitflag_enum = std::integral_constant<bool, is_scoped_enum_v<E>&& is_bitflag_enum_type_v<E>>;

    template<typename E>
    constexpr bool is_bitflag_enum_v = is_bitflag_enum<E>::value;


    template<typename E>
    struct is_incrementable_enum_type : std::false_type {};

    template<typename E>
    constexpr bool is_incrementable_enum_type_v = is_incrementable_enum_type<E>::value;

    template<typename E>
    using is_incrementable_enum = std::integral_constant<bool, is_scoped_enum_v<E>&& is_incrementable_enum_type_v<E>>;

    template<typename E>
    constexpr bool is_incrementable_enum_v = is_incrementable_enum<E>::value;


    template<typename E>
    struct is_decrementable_enum_type : std::false_type {};

    template<typename E>
    constexpr bool is_decrementable_enum_type_v = is_decrementable_enum_type<E>::value;

    template<typename E>
    using is_decrementable_enum = std::integral_constant<bool, is_scoped_enum_v<E>&& is_decrementable_enum_type_v<E>>;

    template<typename E>
    constexpr bool is_decrementable_enum_v = is_decrementable_enum<E>::value;

} // namespace TypeUtils


/************************************************************************/
/* BITFLAGS                                                             */
/************************************************************************/

template<typename E, typename = std::enable_if_t<TypeUtils::is_bitflag_enum_v<E>>>
E& operator|=(E& a, const E& b) noexcept {
    using underlying = std::underlying_type_t<E>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    a = static_cast<E>(underlying_a | underlying_b);
    return a;
}

template<typename E, typename = std::enable_if_t<TypeUtils::is_bitflag_enum_v<E>>>
E operator|(E a, const E& b) noexcept {
    a |= b;
    return a;
}

template<typename E, typename = std::enable_if_t<TypeUtils::is_bitflag_enum_v<E>>>
E& operator&=(E& a, const E& b) noexcept {
    using underlying = std::underlying_type_t<E>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    a = static_cast<E>(underlying_a & underlying_b);
    return a;
}

template<typename E, typename = std::enable_if_t<TypeUtils::is_bitflag_enum_v<E>>>
E operator&(E a, const E& b) noexcept {
    a &= b;
    return a;
}

template<typename E, typename = std::enable_if_t<TypeUtils::is_bitflag_enum_v<E>>>
E& operator^=(E& a, const E& b) noexcept {
    using underlying = std::underlying_type_t<E>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    a = static_cast<E>(underlying_a ^ underlying_b);
    return a;
}

template<typename E, typename = std::enable_if_t<TypeUtils::is_bitflag_enum_v<E>>>
E operator^(E a, const E& b) noexcept {
    a ^= b;
    return a;
}

template<typename E, typename = std::enable_if_t<TypeUtils::is_bitflag_enum_v<E>>>
E operator~(E a) noexcept {
    using underlying = std::underlying_type_t<E>;
    auto underlying_a = static_cast<underlying>(a);
    a = static_cast<E>(~underlying_a);
    return a;
}

/************************************************************************/
/* INCREMENTABLE                                                        */
/************************************************************************/

template<typename E, typename = std::enable_if_t<TypeUtils::is_incrementable_enum_v<E>>>
E& operator++(E& e) noexcept {
    using underlying = std::underlying_type_t<E>;
    e = static_cast<E>(static_cast<underlying>(e) + 1);
    if(e == E::Last_) {
        e = E::First_;
    }
    return e;
}

template<typename E, typename = std::enable_if_t<TypeUtils::is_incrementable_enum_v<E>>>
E operator++(E& e, int) noexcept {
    E result = e;
    ++e;
    return result;
}


/************************************************************************/
/* DECREMENTABLE                                                        */
/************************************************************************/

template<typename E, typename = std::enable_if_t<TypeUtils::is_decrementable_enum_v<E>>>
E& operator--(E& e) noexcept {
    if(e == E::First_) {
        e = E::Last_;
    }
    using underlying = std::underlying_type_t<E>;
    e = static_cast<E>(static_cast<underlying>(e) - 1);
    return e;
}

template<typename E, typename = std::enable_if_t<TypeUtils::is_decrementable_enum_v<E>>>
E operator--(E& e, int) noexcept {
    E result = e;
    --e;
    return result;
}

