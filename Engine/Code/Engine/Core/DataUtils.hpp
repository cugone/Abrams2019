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
                        const std::string& optionalAttributes = std::string(""));

unsigned int GetAttributeCount(const XMLElement &element);
unsigned int GetChildElementCount(const XMLElement &element, const std::string& elementName = std::string(""));

std::vector<std::string> GetChildElementNames(const XMLElement& element);
std::vector<std::string> GetAttributeNames(const XMLElement& element);

bool ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, bool defaultValue);

unsigned char ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, unsigned char defaultValue);
signed char ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, signed char defaultValue);
char ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, char defaultValue);

unsigned short ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, unsigned short defaultValue);
short ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, short defaultValue);

unsigned int ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, unsigned int defaultValue);
int ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, int defaultValue);

unsigned long ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, unsigned long defaultValue);
long ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, long defaultValue);

unsigned long long ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, unsigned long long defaultValue);
long long ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, long long defaultValue);

float ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, float defaultValue);
double ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, double defaultValue);
long double ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, long double defaultValue);

Rgba ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const Rgba& defaultValue);

Vector2 ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const Vector2& defaultValue);
IntVector2 ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const IntVector2& defaultValue);

Vector3 ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const Vector3& defaultValue);
IntVector3 ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const IntVector3& defaultValue);

Vector4 ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const Vector4& defaultValue);
IntVector4 ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const IntVector4& defaultValue);

Matrix4 ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const Matrix4& defaultValue);

std::string ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const std::string& defaultValue);
std::string ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const char* defaultValue);

bool ParseXmlElementText(const XMLElement& element, bool defaultValue);

unsigned char ParseXmlElementText(const XMLElement& element, unsigned char defaultValue);
signed char ParseXmlElementText(const XMLElement& element, signed char defaultValue);
char ParseXmlElementText(const XMLElement& element, char defaultValue);

unsigned short ParseXmlElementText(const XMLElement& element, unsigned short defaultValue);
short ParseXmlElementText(const XMLElement& element, short defaultValue);

unsigned int ParseXmlElementText(const XMLElement& element, unsigned int defaultValue);
int ParseXmlElementText(const XMLElement& element, int defaultValue);

unsigned long ParseXmlElementText(const XMLElement& element, unsigned long defaultValue);
long ParseXmlElementText(const XMLElement& element, long defaultValue);

unsigned long long ParseXmlElementText(const XMLElement& element, unsigned long long defaultValue);
long long ParseXmlElementText(const XMLElement& element, long long defaultValue);

float ParseXmlElementText(const XMLElement& element, float defaultValue);
double ParseXmlElementText(const XMLElement& element, double defaultValue);
long double ParseXmlElementText(const XMLElement& element, long double defaultValue);

Rgba ParseXmlElementText(const XMLElement& element, const Rgba& defaultValue);

Vector2 ParseXmlElementText(const XMLElement& element, const Vector2& defaultValue);
IntVector2 ParseXmlElementText(const XMLElement& element, const IntVector2& defaultValue);

Vector3 ParseXmlElementText(const XMLElement& element, const Vector3& defaultValue);
IntVector3 ParseXmlElementText(const XMLElement& element, const IntVector3& defaultValue);

Vector4 ParseXmlElementText(const XMLElement& element, const Vector4& defaultValue);
IntVector4 ParseXmlElementText(const XMLElement& element, const IntVector4& defaultValue);

Matrix4 ParseXmlElementText(const XMLElement& element, const Matrix4& defaultValue);

std::string ParseXmlElementText(const XMLElement& element, const char* defaultValue);
std::string ParseXmlElementText(const XMLElement& element, const std::string& defaultValue);

void ForEachChildElement(const XMLElement& element, const std::string& childname = std::string{}, const std::function<void(const XMLElement&)>& callback = [](const XMLElement&) { /* DO NOTHING */ });
void ForEachAttribute(const XMLElement& element, const std::function<void(const XMLAttribute&)>& callback = [](const XMLAttribute&) { /* DO NOTHING */ });

}
