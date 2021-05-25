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

[[nodiscard]] bool ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, bool defaultValue) noexcept;

[[nodiscard]] unsigned char ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, unsigned char defaultValue) noexcept;
[[nodiscard]] signed char ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, signed char defaultValue) noexcept;
[[nodiscard]] char ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, char defaultValue) noexcept;

[[nodiscard]] unsigned short ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, unsigned short defaultValue) noexcept;
[[nodiscard]] short ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, short defaultValue) noexcept;

[[nodiscard]] unsigned int ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, unsigned int defaultValue) noexcept;
[[nodiscard]] int ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, int defaultValue) noexcept;

[[nodiscard]] unsigned long ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, unsigned long defaultValue) noexcept;
[[nodiscard]] long ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, long defaultValue) noexcept;

[[nodiscard]] unsigned long long ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, unsigned long long defaultValue) noexcept;
[[nodiscard]] long long ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, long long defaultValue) noexcept;

[[nodiscard]] float ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, float defaultValue) noexcept;
[[nodiscard]] double ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, double defaultValue) noexcept;
[[nodiscard]] long double ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, long double defaultValue) noexcept;

[[nodiscard]] Rgba ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const Rgba& defaultValue) noexcept;

[[nodiscard]] Vector2 ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const Vector2& defaultValue) noexcept;
[[nodiscard]] IntVector2 ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const IntVector2& defaultValue) noexcept;

[[nodiscard]] Vector3 ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const Vector3& defaultValue) noexcept;
[[nodiscard]] IntVector3 ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const IntVector3& defaultValue) noexcept;

[[nodiscard]] Vector4 ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const Vector4& defaultValue) noexcept;
[[nodiscard]] IntVector4 ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const IntVector4& defaultValue) noexcept;

[[nodiscard]] Matrix4 ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const Matrix4& defaultValue) noexcept;

[[nodiscard]] std::string ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const std::string& defaultValue) noexcept;
[[nodiscard]] std::string ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const char* defaultValue) noexcept;

[[nodiscard]] bool ParseXmlElementText(const XMLElement& element, bool defaultValue) noexcept;

[[nodiscard]] unsigned char ParseXmlElementText(const XMLElement& element, unsigned char defaultValue) noexcept;
[[nodiscard]] signed char ParseXmlElementText(const XMLElement& element, signed char defaultValue) noexcept;
[[nodiscard]] char ParseXmlElementText(const XMLElement& element, char defaultValue) noexcept;

[[nodiscard]] unsigned short ParseXmlElementText(const XMLElement& element, unsigned short defaultValue) noexcept;
[[nodiscard]] short ParseXmlElementText(const XMLElement& element, short defaultValue) noexcept;

[[nodiscard]] unsigned int ParseXmlElementText(const XMLElement& element, unsigned int defaultValue) noexcept;
[[nodiscard]] int ParseXmlElementText(const XMLElement& element, int defaultValue) noexcept;

[[nodiscard]] unsigned long ParseXmlElementText(const XMLElement& element, unsigned long defaultValue) noexcept;
[[nodiscard]] long ParseXmlElementText(const XMLElement& element, long defaultValue) noexcept;

[[nodiscard]] unsigned long long ParseXmlElementText(const XMLElement& element, unsigned long long defaultValue) noexcept;
[[nodiscard]] long long ParseXmlElementText(const XMLElement& element, long long defaultValue) noexcept;

[[nodiscard]] float ParseXmlElementText(const XMLElement& element, float defaultValue) noexcept;
[[nodiscard]] double ParseXmlElementText(const XMLElement& element, double defaultValue) noexcept;
[[nodiscard]] long double ParseXmlElementText(const XMLElement& element, long double defaultValue) noexcept;

[[nodiscard]] Rgba ParseXmlElementText(const XMLElement& element, const Rgba& defaultValue) noexcept;

[[nodiscard]] Vector2 ParseXmlElementText(const XMLElement& element, const Vector2& defaultValue) noexcept;
[[nodiscard]] IntVector2 ParseXmlElementText(const XMLElement& element, const IntVector2& defaultValue) noexcept;

[[nodiscard]] Vector3 ParseXmlElementText(const XMLElement& element, const Vector3& defaultValue) noexcept;
[[nodiscard]] IntVector3 ParseXmlElementText(const XMLElement& element, const IntVector3& defaultValue) noexcept;

[[nodiscard]] Vector4 ParseXmlElementText(const XMLElement& element, const Vector4& defaultValue) noexcept;
[[nodiscard]] IntVector4 ParseXmlElementText(const XMLElement& element, const IntVector4& defaultValue) noexcept;

[[nodiscard]] Matrix4 ParseXmlElementText(const XMLElement& element, const Matrix4& defaultValue) noexcept;

[[nodiscard]] std::string ParseXmlElementText(const XMLElement& element, const char* defaultValue) noexcept;
[[nodiscard]] std::string ParseXmlElementText(const XMLElement& element, const std::string& defaultValue) noexcept;

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
    for(auto xml_iter = element.FirstChildElement(childNameAsCStr); xml_iter != nullptr; xml_iter = xml_iter->NextSiblingElement(childNameAsCStr)) {
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
    for(auto attribute = element.FirstAttribute(); attribute != nullptr; attribute = attribute->Next()) {
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

} // namespace DataUtils
