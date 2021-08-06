#pragma once

#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/IntVector3.hpp"
#include "Engine/Math/IntVector4.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Matrix4.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"

#include <Thirdparty/TinyXML2/tinyxml2.h>

#include <functional>
#include <intrin.h>
#include <string>

using XMLElement = tinyxml2::XMLElement;
using XMLAttribute = tinyxml2::XMLAttribute;

namespace DataUtils {

[[nodiscard]] constexpr inline auto Bits(uint8_t value) noexcept -> uint8_t {
    const char* const bits =
    "\0\1\1\2\1\2\2\3\1\2\2\3\2\3\3\4"
    "\1\2\2\3\2\3\3\4\2\3\3\4\3\4\4\5"
    "\1\2\2\3\2\3\3\4\2\3\3\4\3\4\4\5"
    "\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
    "\1\2\2\3\2\3\3\4\2\3\3\4\3\4\4\5"
    "\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
    "\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
    "\3\4\4\5\4\5\5\6\4\5\5\6\5\6\6\7"
    "\1\2\2\3\2\3\3\4\2\3\3\4\3\4\4\5"
    "\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
    "\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
    "\3\4\4\5\4\5\5\6\4\5\5\6\5\6\6\7"
    "\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
    "\3\4\4\5\4\5\5\6\4\5\5\6\5\6\6\7"
    "\3\4\4\5\4\5\5\6\4\5\5\6\5\6\6\7"
    "\4\5\5\6\5\6\6\7\5\6\6\7\6\7\7\x8";
    return bits[value];
}

[[nodiscard]] inline auto Bits(uint16_t value) noexcept -> uint16_t {
    //TODO: Use <bit> header version (std::popcount) when it becomes available.
    return __popcnt16(value);
}

[[nodiscard]] inline auto Bits(uint32_t value) noexcept -> uint32_t {
    //TODO: Use <bit> header version (std::popcount) when it becomes available.
    return __popcnt(value);
}

[[nodiscard]] inline auto Bits(uint64_t value) noexcept -> uint64_t {
    //TODO: Use <bit> header version (std::popcount) when it becomes available.
    return __popcnt64(value);
}

[[nodiscard]] constexpr inline auto ShiftLeft(uint8_t value, uint8_t distance) noexcept -> uint8_t {
    return value << distance;
}

[[nodiscard]] constexpr inline auto ShiftLeft(uint16_t value, uint16_t distance) noexcept -> uint16_t {
    return value << distance;
}

[[nodiscard]] constexpr inline auto ShiftLeft(uint32_t value, uint32_t distance) noexcept -> uint32_t {
    return value << distance;
}

[[nodiscard]] constexpr inline auto ShiftLeft(uint64_t value, uint64_t distance) noexcept -> uint64_t {
    return value << distance;
}

[[nodiscard]] constexpr inline auto ShiftRight(uint8_t value, uint8_t distance) noexcept -> uint8_t {
    return value >> distance;
}

[[nodiscard]] constexpr inline auto ShiftRight(uint16_t value, uint16_t distance) noexcept -> uint16_t {
    return value >> distance;
}

[[nodiscard]] constexpr inline auto ShiftRight(uint32_t value, uint32_t distance) noexcept -> uint32_t {
    return value >> distance;
}

[[nodiscard]] constexpr inline auto ShiftRight(uint64_t value, uint64_t distance) noexcept -> uint64_t {
    return value >> distance;
}

[[nodiscard]] constexpr inline auto Bit(uint8_t n) noexcept -> uint8_t {
    return ShiftLeft(1, n);
}

[[nodiscard]] constexpr inline auto Bit(uint16_t n) noexcept -> uint16_t {
    return ShiftLeft(1, n);
}

[[nodiscard]] constexpr inline auto Bit(uint32_t n) noexcept -> uint32_t {
    return ShiftLeft(1, n);
}

[[nodiscard]] constexpr inline auto Bit(uint64_t n) noexcept -> uint64_t {
    return ShiftLeft(1, n);
}

//Unconditional byte order swap.
[[nodiscard]] inline auto EndianSwap(uint16_t value) noexcept -> uint16_t {
    return _byteswap_ushort(value);
}

//Unconditional byte order swap.
[[nodiscard]] inline auto EndianSwap(uint32_t value) noexcept -> uint32_t {
    return _byteswap_ulong(value);
}

//Unconditional byte order swap.
[[nodiscard]] inline auto EndianSwap(uint64_t value) noexcept -> uint64_t {
    return _byteswap_uint64(value);
}

void ValidateXmlElement(const XMLElement& element,
                        const std::string& name,
                        const std::string& requiredChildElements,
                        const std::string& requiredAttributes,
                        const std::string& optionalChildElements = std::string{},
                        const std::string& optionalAttributes = std::string{}) noexcept;

std::string GetElementTextAsString(const XMLElement& element);
std::string GetAttributeAsString(const XMLElement& element, const std::string& attributeName);

[[nodiscard]] std::size_t GetAttributeCount(const XMLElement& element) noexcept;
[[nodiscard]] std::size_t GetChildElementCount(const XMLElement& element, const std::string& elementName = std::string{}) noexcept;

[[nodiscard]] std::string GetElementName(const XMLElement& elem) noexcept;
[[nodiscard]] std::vector<std::string> GetChildElementNames(const XMLElement& element) noexcept;
[[nodiscard]] bool HasChild(const XMLElement& elem) noexcept;
[[nodiscard]] bool HasChild(const XMLElement& elem, const std::string& name) noexcept;

[[nodiscard]] std::string GetAttributeName(const XMLAttribute& attrib) noexcept;
[[nodiscard]] std::vector<std::string> GetAttributeNames(const XMLElement& element) noexcept;
[[nodiscard]] bool HasAttribute(const XMLElement& element) noexcept;
[[nodiscard]] bool HasAttribute(const XMLElement& element, const std::string& name);

//************************************
// Method:    ForEachChildElement
// FullName:  DataUtils::ForEachChildElement
// Access:    public
// Returns:   UnaryFunction: A copy of the UnaryFunction Callable argument.
// Qualifier: noexcept
// Parameter: const XMLElement& element: The parent element.
// Parameter: const std::string& childname: The name of the child element to iterate. Provide an empty string to iterate over all children.
// Parameter: UnaryFunction&& f: UnaryFunction Callable to invoke for each child element of the parent. Must be of the signature void f(XMLElement&). cv-qualified is optional.
//************************************
template<typename UnaryFunction>
UnaryFunction ForEachChildElement(const XMLElement& element, const std::string& childname, UnaryFunction&& f) noexcept {
    auto childNameAsCStr = childname.empty() ? nullptr : childname.c_str();
    for(auto* xml_iter = element.FirstChildElement(childNameAsCStr); xml_iter != nullptr; xml_iter = xml_iter->NextSiblingElement(childNameAsCStr)) {
        std::invoke(f, *xml_iter);
    }
    return f;
}

//************************************
// Method:    ForEachAttribute
// FullName:  DataUtils::ForEachAttribute
// Access:    public
// Returns:   UnaryFunction: A copy of the UnaryFunction Callable argument.
// Qualifier: noexcept
// Parameter: const XMLElement& element: The parent element.
// Parameter: UnaryFunction&& f: UnaryFunction Callable to invoke for each attribute of the parent element. Must be of the signature void f(XMLAttribute&). cv-qualified is optional.
//************************************
template<typename UnaryFunction>
UnaryFunction ForEachAttribute(const XMLElement& element, UnaryFunction&& f) noexcept {
    for(auto* attribute = element.FirstAttribute(); attribute != nullptr; attribute = attribute->Next()) {
        std::invoke(f, *attribute);
    }
    return f;
}

namespace detail {
template<typename T>
[[nodiscard]] const T CalculateIntegerRangeResult(const std::string& txt) {
    const auto values = StringUtils::Split(txt, '~');
    if(values.empty() && !txt.empty()) {
        constexpr auto lower = (std::numeric_limits<T>::min)();
        constexpr auto upper = (std::numeric_limits<T>::max)();
        return static_cast<T>(MathUtils::GetRandomIntInRange(lower, upper));
    }
    if(values.size() == 1) {
        if(txt.front() == '~') {
            constexpr auto lower = (std::numeric_limits<T>::min)();
            const auto upper = static_cast<T>(std::stoi(values[1]));
            return static_cast<T>(MathUtils::GetRandomIntInRange(lower, upper));
        }
        if(txt.back() == '~') {
            const auto lower = static_cast<T>(std::stoi(values[0]));
            constexpr auto upper = (std::numeric_limits<T>::max)();
            return static_cast<T>(MathUtils::GetRandomIntInRange(lower, upper));
        }
        return static_cast<T>(std::stoi(values[0]));
    }
    const auto lower = static_cast<T>(std::stoi(values[0]));
    const auto upper = static_cast<T>(std::stoi(values[1]));
    return static_cast<T>(MathUtils::GetRandomIntInRange(lower, upper));
}

template<typename T>
[[nodiscard]] const T CalculateUnsignedIntegerRangeResult(const std::string& txt) {
    const auto values = StringUtils::Split(txt, '~');
    if(values.empty() && !txt.empty()) {
        constexpr auto lower = (std::numeric_limits<T>::min)();
        constexpr auto upper = (std::numeric_limits<T>::max)();
        return static_cast<T>(MathUtils::GetRandomIntInRange(lower, upper));
    }
    if(values.size() == 1) {
        if(txt.front() == '~') {
            constexpr auto lower = (std::numeric_limits<T>::min)();
            const auto upper = static_cast<T>(std::stoul(values[1]));
            return static_cast<T>(MathUtils::GetRandomIntInRange(lower, upper));
        }
        if(txt.back() == '~') {
            const auto lower = static_cast<T>(std::stoul(values[0]));
            constexpr auto upper = (std::numeric_limits<T>::max)();
            return static_cast<T>(MathUtils::GetRandomIntInRange(lower, upper));
        }
        return static_cast<T>(std::stoul(values[0]));
    }
    const auto lower = static_cast<T>(std::stoul(values[0]));
    const auto upper = static_cast<T>(std::stoul(values[1]));
    return static_cast<T>(MathUtils::GetRandomIntInRange(lower, upper));
}

template<typename T>
[[nodiscard]] const T CalculateLongLongRangeResult(const std::string& txt) {
    const auto values = StringUtils::Split(txt, '~');
    if(values.empty() && !txt.empty()) {
        constexpr auto lower = (std::numeric_limits<T>::min)();
        constexpr auto upper = (std::numeric_limits<T>::max)();
        return static_cast<T>(MathUtils::GetRandomLongLongInRange(lower, upper));
    }
    if(values.size() == 1) {
        if(txt.front() == '~') {
            constexpr auto lower = (std::numeric_limits<T>::min)();
            const auto upper = static_cast<T>(std::stoll(values[1]));
            return static_cast<T>(MathUtils::GetRandomLongLongInRange(lower, upper));
        }
        if(txt.back() == '~') {
            const auto lower = static_cast<T>(std::stoll(values[0]));
            constexpr auto upper = (std::numeric_limits<T>::max)();
            return static_cast<T>(MathUtils::GetRandomLongLongInRange(lower, upper));
        }
        return static_cast<T>(std::stoll(values[0]));
    }
    const auto lower = static_cast<T>(std::stoll(values[0]));
    const auto upper = static_cast<T>(std::stoll(values[1]));
    return static_cast<T>(MathUtils::GetRandomLongLongInRange(lower, upper));
}

template<typename T>
[[nodiscard]] const T CalculateUnsignedLongLongRangeResult(const std::string& txt) {
    const auto values = StringUtils::Split(txt, '~');
    if(values.empty() && !txt.empty()) {
        constexpr auto lower = (std::numeric_limits<T>::min)();
        constexpr auto upper = (std::numeric_limits<T>::max)();
        return static_cast<T>(MathUtils::GetRandomLongLongInRange(lower, upper));
    }
    if(values.size() == 1) {
        if(txt.front() == '~') {
            constexpr auto lower = (std::numeric_limits<T>::min)();
            const auto upper = static_cast<T>(std::stoul(values[1]));
            return static_cast<T>(MathUtils::GetRandomLongLongInRange(lower, upper));
        }
        if(txt.back() == '~') {
            const auto lower = static_cast<T>(std::stoul(values[0]));
            constexpr auto upper = (std::numeric_limits<T>::max)();
            return static_cast<T>(MathUtils::GetRandomLongLongInRange(lower, upper));
        }
        return static_cast<T>(std::stoul(values[0]));
    }
    const auto lower = static_cast<T>(std::stoul(values[0]));
    const auto upper = static_cast<T>(std::stoul(values[1]));
    return static_cast<T>(MathUtils::GetRandomLongLongInRange(lower, upper));
}

template<typename T>
[[nodiscard]] const T CalculateFloatRangeResult(const std::string& txt) {
    const auto values = StringUtils::Split(txt, '~');
    if(values.empty() && !txt.empty()) {
        constexpr auto lower = (std::numeric_limits<T>::min)();
        constexpr auto upper = (std::numeric_limits<T>::max)();
        return static_cast<T>(MathUtils::GetRandomFloatInRange(lower, upper));
    }
    if(values.size() == 1) {
        if(txt.front() == '~') {
            constexpr auto lower = (std::numeric_limits<T>::min)();
            const auto upper = static_cast<T>(std::stof(values[1]));
            return static_cast<T>(MathUtils::GetRandomFloatInRange(lower, upper));
        }
        if(txt.back() == '~') {
            const auto lower = static_cast<T>(std::stof(values[0]));
            constexpr auto upper = (std::numeric_limits<T>::max)();
            return static_cast<T>(MathUtils::GetRandomFloatInRange(lower, upper));
        }
        return static_cast<T>(std::stof(values[0]));
    }
    const auto lower = static_cast<T>(std::stof(values[0]));
    const auto upper = static_cast<T>(std::stof(values[1]));
    return static_cast<T>(MathUtils::GetRandomFloatInRange(lower, upper));
}

template<typename T>
[[nodiscard]] const T CalculateDoubleRangeResult(const std::string& txt) {
    const auto values = StringUtils::Split(txt, '~');
    if(values.empty() && !txt.empty()) {
        constexpr auto lower = (std::numeric_limits<T>::min)();
        constexpr auto upper = (std::numeric_limits<T>::max)();
        return static_cast<T>(MathUtils::GetRandomDoubleInRange(lower, upper));
    }
    if(values.size() == 1) {
        if(txt.front() == '~') {
            constexpr auto lower = (std::numeric_limits<T>::min)();
            const auto upper = static_cast<T>(std::stod(values[1]));
            return static_cast<T>(MathUtils::GetRandomDoubleInRange(lower, upper));
        }
        if(txt.back() == '~') {
            const auto lower = static_cast<T>(std::stod(values[0]));
            constexpr auto upper = (std::numeric_limits<T>::max)();
            return static_cast<T>(MathUtils::GetRandomDoubleInRange(lower, upper));
        }
        return static_cast<T>(std::stod(values[0]));
    }
    const auto lower = static_cast<T>(std::stod(values[0]));
    const auto upper = static_cast<T>(std::stod(values[1]));
    return static_cast<T>(MathUtils::GetRandomDoubleInRange(lower, upper));
}

template<typename T>
[[nodiscard]] const T CalculateLongDoubleRangeResult(const std::string& txt) {
    const auto values = StringUtils::Split(txt, '~');
    if(values.empty() && !txt.empty()) {
        constexpr auto lower = (std::numeric_limits<T>::min)();
        constexpr auto upper = (std::numeric_limits<T>::max)();
        return static_cast<T>(MathUtils::GetRandomLongDoubleInRange(lower, upper));
    }
    if(values.size() == 1) {
        if(txt.front() == '~') {
            constexpr auto lower = (std::numeric_limits<T>::min)();
            const auto upper = static_cast<T>(std::stold(values[1]));
            return static_cast<T>(MathUtils::GetRandomLongDoubleInRange(lower, upper));
        }
        if(txt.back() == '~') {
            const auto lower = static_cast<T>(std::stold(values[0]));
            constexpr auto upper = (std::numeric_limits<T>::max)();
            return static_cast<T>(MathUtils::GetRandomLongDoubleInRange(lower, upper));
        }
        return static_cast<T>(std::stold(values[0]));
    }
    const auto lower = static_cast<T>(std::stold(values[0]));
    const auto upper = static_cast<T>(std::stold(values[1]));
    return static_cast<T>(MathUtils::GetRandomLongDoubleInRange(lower, upper));
}

} // namespace detail

template<typename T>
[[nodiscard]] T ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, T defaultValue) noexcept {
    auto retVal = defaultValue;
    const auto attr = GetAttributeAsString(element, attributeName);
    const auto is_range = attr.find('~') != std::string::npos;
    if(!is_range) {
        try {
            if constexpr(std::is_same_v<T, bool>) {
                element.QueryBoolAttribute(attributeName.c_str(), &retVal);
            } else if constexpr(std::is_same_v<T, unsigned char>) {
                auto ucharVal = static_cast<unsigned int>(retVal);
                element.QueryUnsignedAttribute(attributeName.c_str(), &ucharVal);
                return static_cast<T>(ucharVal);
            } else if constexpr(std::is_same_v<T, char>) {
                const auto* s = element.Attribute(attributeName.c_str());
                return T(s ? s[0] : defaultValue);
            } else if constexpr(std::is_same_v<T, signed char>) {
                auto scharVal = static_cast<signed int>(retVal);
                element.QueryIntAttribute(attributeName.c_str(), &scharVal);
                return static_cast<T>(scharVal);
            } else if constexpr(std::is_same_v<T, std::string>) {
                const auto* s = element.Attribute(attributeName.c_str());
                return T(s ? s : defaultValue);
            } else if constexpr(std::is_same_v<T, const char*>) {
                const auto* s = element.Attribute(attributeName.c_str());
                return s ? s : "";
            } else if constexpr(std::is_unsigned_v<T> && std::is_same_v<T, std::uint64_t>) {
                element.QueryUnsigned64Attribute(attributeName.c_str(), &retVal);
            } else if constexpr(std::is_unsigned_v<T> && std::is_same_v<T, std::uint32_t>) {
                element.QueryUnsignedAttribute(attributeName.c_str(), &retVal);
            } else if constexpr(std::is_unsigned_v<T>) {
                element.QueryUnsignedAttribute(attributeName.c_str(), &retVal);
            } else if constexpr(std::is_signed_v<T> && std::is_same_v<T, std::int64_t>) {
                element.QueryInt64Attribute(attributeName.c_str(), &retVal);
            } else if constexpr(std::is_signed_v<T> && std::is_same_v<T, std::int32_t>) {
                element.QueryIntAttribute(attributeName.c_str(), &retVal);
            } else if constexpr(std::is_signed_v<T> && !std::is_floating_point_v<T>) {
                element.QueryIntAttribute(attributeName.c_str(), &retVal);
            } else if constexpr(std::is_floating_point_v<T> && std::is_same_v<T, float>) {
                element.QueryFloatAttribute(attributeName.c_str(), &retVal);
            } else if constexpr(std::is_floating_point_v<T> && std::is_same_v<T, double>) {
                element.QueryDoubleAttribute(attributeName.c_str(), &retVal);
            } else if constexpr(std::is_floating_point_v<T> && std::is_same_v<T, long double>) {
                element.QueryDoubleAttribute(attributeName.c_str(), &retVal);
            } else {
                if(attr.empty()) {
                    return defaultValue;
                } else {
                    return T{attr};
                }
            }
        } catch(...) {
            return defaultValue;
        }
    } else {
        const auto values = StringUtils::Split(attr, '~');
        if(values.size() == 1) {
            if constexpr(std::is_unsigned_v<T>) {
                return static_cast<T>(std::stoul(values[0]));
            } else if constexpr(std::is_signed_v<T> && !std::is_floating_point_v<T>) {
                return static_cast<T>(std::stoll(values[0]));
            } else if constexpr(std::is_signed_v<T> && std::is_floating_point_v<T>) {
                return static_cast<T>(std::stold(values[0]));
            } else {
                if(attr.empty()) {
                    return defaultValue;
                } else {
                    return T(attr);
                }
            }
        }
        if constexpr(std::is_unsigned_v<T>) {
            retVal = static_cast<T>(detail::CalculateUnsignedLongLongRangeResult<decltype(retVal)>(attr));
        } else if constexpr(std::is_signed_v<T> && !std::is_floating_point_v<T>) {
            retVal = static_cast<T>(detail::CalculateLongLongRangeResult<decltype(retVal)>(attr));
        } else if constexpr(std::is_floating_point_v<T>) {
            retVal = static_cast<T>(detail::CalculateLongDoubleRangeResult<decltype(retVal)>(attr));
        }
    }
    return retVal;
}

template<typename T>
[[nodiscard]] T ParseXmlElementText(const XMLElement& element, T defaultValue) noexcept {
    auto retVal = defaultValue;
    using R = decltype(retVal);
    const auto txt = GetElementTextAsString(element);
    const auto is_range = txt.find('~') != std::string::npos;
    if(!is_range) {
        try {
            if constexpr(std::is_same_v<T, bool>) {
                const auto txt_bool = StringUtils::ToLowerCase(txt);
                if(txt_bool == "true") {
                    return true;
                } else if(txt_bool == "false") {
                    return false;
                } else {
                    try {
                        return static_cast<R>(std::stoi(txt_bool));
                    } catch(...) {
                        return defaultValue;
                    }
                }
            } else if constexpr(std::is_unsigned_v<T>) {
                retVal = static_cast<R>(std::stoul(txt));
            } else if constexpr(std::is_signed_v<T> && !std::is_floating_point_v<T>) {
                retVal = static_cast<R>(std::stoll(txt));
            } else if constexpr(std::is_signed_v<T> && std::is_floating_point_v<T>) {
                retVal = static_cast<R>(std::stold(txt));
            }
        } catch(...) {
            return defaultValue;
        }
    } else {
        if constexpr(std::is_unsigned_v<T>) {
            retVal = static_cast<R>(detail::CalculateUnsignedLongLongRangeResult<R>(txt));
        } else if constexpr(std::is_signed_v<T> && !std::is_floating_point_v<T>) {
            retVal = static_cast<R>(detail::CalculateLongLongRangeResult<R>(txt));
        } else if constexpr(std::is_signed_v<T> && std::is_floating_point_v<T>) {
            retVal = static_cast<R>(detail::CalculateLongDoubleRangeResult<R>(txt));
        } else {
            if(txt.empty()) {
                return defaultValue;
            } else {
                return T{txt};
            }
        }
    }
    return retVal;
}

} // namespace DataUtils
