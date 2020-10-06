#pragma once

#include "Engine/Core/TypeUtils.hpp"

#include <bitset>
#include <string>

class Rgba;
class Vector2;
class Vector3;
class Vector4;
class IntVector2;
class IntVector3;
class IntVector4;
class Matrix4;

enum class ArgumentParserState : uint8_t {
    None,
    BadBit,
    FailBit,
    EndOfFileBit,
    Max
};

template<>
struct TypeUtils::is_bitflag_enum_type<ArgumentParserState> : std::true_type {};

class ArgumentParser {
public:
    explicit ArgumentParser(const std::string& args) noexcept;
    template<typename T>
    friend ArgumentParser& operator>>(ArgumentParser& parser, T&& arg) noexcept;
    void clear() noexcept;
    [[nodiscard]] bool fail() const noexcept;
    [[nodiscard]] bool good() const noexcept;
    [[nodiscard]] bool bad() const noexcept;
    [[nodiscard]] bool eof() const noexcept;
    [[nodiscard]] operator bool() const noexcept;
    [[nodiscard]] bool operator!() const noexcept;
    bool GetNext(Rgba& value) const noexcept;
    bool GetNext(Vector2& value) const noexcept;
    bool GetNext(Vector3& value) const noexcept;
    bool GetNext(Vector4& value) const noexcept;
    bool GetNext(IntVector2& value) const noexcept;
    bool GetNext(IntVector3& value) const noexcept;
    bool GetNext(IntVector4& value) const noexcept;
    bool GetNext(Matrix4& value) const noexcept;
    [[nodiscard]] bool GetNext(std::string& value) const noexcept;
    bool GetNext(bool& value) const noexcept;
    bool GetNext(unsigned char& value) const noexcept;
    bool GetNext(signed char& value) const noexcept;
    bool GetNext(char& value) const noexcept;
    bool GetNext(unsigned short& value) const noexcept;
    bool GetNext(short& value) const noexcept;
    bool GetNext(unsigned int& value) const noexcept;
    bool GetNext(int& value) const noexcept;
    bool GetNext(unsigned long& value) const noexcept;
    bool GetNext(long& value) const noexcept;
    bool GetNext(unsigned long long& value) const noexcept;
    bool GetNext(long long& value) const noexcept;
    bool GetNext(float& value) const noexcept;
    bool GetNext(double& value) const noexcept;
    bool GetNext(long double& value) const noexcept;

protected:
private:
    void SetState(const ArgumentParserState& stateBits, bool newValue) const noexcept;
    bool GetNextValueFromBuffer(std::string& value) const noexcept;
    mutable std::string _current{};
    mutable std::bitset<static_cast<std::size_t>(ArgumentParserState::Max)> _state_bits{};
};

template<typename T>
ArgumentParser& operator>>(ArgumentParser& parser, T&& arg) noexcept {
    (void)parser.GetNext(std::forward<T>(arg)); //GetNext sets the state of the parser.
    return parser; //It is implicitly converted to bool, a bad state from the previous read is false.
}
