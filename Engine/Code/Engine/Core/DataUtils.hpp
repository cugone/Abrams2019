#pragma once

#include "ThirdParty/TinyXML2/tinyxml2.h"

#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/IntVector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/IntVector4.hpp"
#include "Engine/Math/Matrix4.hpp"

#include <functional>
#include <string>

#include <intrin.h>

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
    return __popcnt16(value);
}

[[nodiscard]] inline auto Bits(uint32_t value) noexcept -> uint32_t {
    return __popcnt(value);
}

[[nodiscard]] inline auto Bits(uint64_t value) noexcept -> uint64_t {
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

void ValidateXmlElement(const XMLElement& element,
                        const std::string& name,
                        const std::string& requiredChildElements,
                        const std::string& requiredAttributes,
                        const std::string& optionalChildElements = std::string(""),
                        const std::string& optionalAttributes = std::string("")) noexcept;

unsigned int GetAttributeCount(const XMLElement &element) noexcept;
unsigned int GetChildElementCount(const XMLElement &element, const std::string& elementName = std::string("")) noexcept;

std::string GetElementName(const XMLElement& elem) noexcept;
std::vector<std::string> GetChildElementNames(const XMLElement& element) noexcept;
std::string GetAttributeName(const XMLAttribute& attrib) noexcept;
std::vector<std::string> GetAttributeNames(const XMLElement& element) noexcept;

bool ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, bool defaultValue) noexcept;

unsigned char ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, unsigned char defaultValue) noexcept;
signed char ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, signed char defaultValue) noexcept;
char ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, char defaultValue) noexcept;

unsigned short ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, unsigned short defaultValue) noexcept;
short ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, short defaultValue) noexcept;

unsigned int ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, unsigned int defaultValue) noexcept;
int ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, int defaultValue) noexcept;

unsigned long ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, unsigned long defaultValue) noexcept;
long ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, long defaultValue) noexcept;

unsigned long long ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, unsigned long long defaultValue) noexcept;
long long ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, long long defaultValue) noexcept;

float ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, float defaultValue) noexcept;
double ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, double defaultValue) noexcept;
long double ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, long double defaultValue) noexcept;

Rgba ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const Rgba& defaultValue) noexcept;

Vector2 ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const Vector2& defaultValue) noexcept;
IntVector2 ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const IntVector2& defaultValue) noexcept;

Vector3 ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const Vector3& defaultValue) noexcept;
IntVector3 ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const IntVector3& defaultValue) noexcept;

Vector4 ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const Vector4& defaultValue) noexcept;
IntVector4 ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const IntVector4& defaultValue) noexcept;

Matrix4 ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const Matrix4& defaultValue) noexcept;

std::string ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const std::string& defaultValue) noexcept;
std::string ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const char* defaultValue) noexcept;

bool ParseXmlElementText(const XMLElement& element, bool defaultValue) noexcept;

unsigned char ParseXmlElementText(const XMLElement& element, unsigned char defaultValue) noexcept;
signed char ParseXmlElementText(const XMLElement& element, signed char defaultValue) noexcept;
char ParseXmlElementText(const XMLElement& element, char defaultValue) noexcept;

unsigned short ParseXmlElementText(const XMLElement& element, unsigned short defaultValue) noexcept;
short ParseXmlElementText(const XMLElement& element, short defaultValue) noexcept;

unsigned int ParseXmlElementText(const XMLElement& element, unsigned int defaultValue) noexcept;
int ParseXmlElementText(const XMLElement& element, int defaultValue) noexcept;

unsigned long ParseXmlElementText(const XMLElement& element, unsigned long defaultValue) noexcept;
long ParseXmlElementText(const XMLElement& element, long defaultValue) noexcept;

unsigned long long ParseXmlElementText(const XMLElement& element, unsigned long long defaultValue) noexcept;
long long ParseXmlElementText(const XMLElement& element, long long defaultValue) noexcept;

float ParseXmlElementText(const XMLElement& element, float defaultValue) noexcept;
double ParseXmlElementText(const XMLElement& element, double defaultValue) noexcept;
long double ParseXmlElementText(const XMLElement& element, long double defaultValue) noexcept;

Rgba ParseXmlElementText(const XMLElement& element, const Rgba& defaultValue) noexcept;

Vector2 ParseXmlElementText(const XMLElement& element, const Vector2& defaultValue) noexcept;
IntVector2 ParseXmlElementText(const XMLElement& element, const IntVector2& defaultValue) noexcept;

Vector3 ParseXmlElementText(const XMLElement& element, const Vector3& defaultValue) noexcept;
IntVector3 ParseXmlElementText(const XMLElement& element, const IntVector3& defaultValue) noexcept;

Vector4 ParseXmlElementText(const XMLElement& element, const Vector4& defaultValue) noexcept;
IntVector4 ParseXmlElementText(const XMLElement& element, const IntVector4& defaultValue) noexcept;

Matrix4 ParseXmlElementText(const XMLElement& element, const Matrix4& defaultValue) noexcept;

std::string ParseXmlElementText(const XMLElement& element, const char* defaultValue) noexcept;
std::string ParseXmlElementText(const XMLElement& element, const std::string& defaultValue) noexcept;

template<typename Callable, typename ...Args>
void ForEachChildElement(const XMLElement& element, const std::string& childname, Callable&& callback, Args&& ...args) noexcept {
    auto childNameAsCStr = childname.empty() ? nullptr : childname.c_str();
    for(auto xml_iter = element.FirstChildElement(childNameAsCStr);
        xml_iter != nullptr;
        xml_iter = xml_iter->NextSiblingElement(childNameAsCStr)) {
        std::invoke(callback, *xml_iter, std::forward<Args>(args)...);
    }
}

template<typename Callable, typename ...Args>
void ForEachAttribute(const XMLElement& element, Callable&& callback, Args&& ...args) noexcept {
    for(auto attribute = element.FirstAttribute(); attribute != nullptr; attribute = attribute->Next()) {
        std::invoke(callback, *attribute, std::forward<Args>(args)...);
    }
}


}
