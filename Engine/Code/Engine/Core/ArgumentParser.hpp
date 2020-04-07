#pragma once

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

class ArgumentParser {
public:
    explicit ArgumentParser(const std::string& args) noexcept;
    template<typename T>
    friend ArgumentParser& operator>>(ArgumentParser& parser, T&& arg) noexcept;
    void clear() noexcept;
    bool fail() const noexcept;
    bool good() const noexcept;
    bool bad() const noexcept;
    bool eof() const noexcept;
    operator bool() const noexcept;
    bool operator!() const noexcept;
    bool GetNext(Rgba& value) const noexcept;
    bool GetNext(Vector2& value) const noexcept;
    bool GetNext(Vector3& value) const noexcept;
    bool GetNext(Vector4& value) const noexcept;
    bool GetNext(IntVector2& value) const noexcept;
    bool GetNext(IntVector3& value) const noexcept;
    bool GetNext(IntVector4& value) const noexcept;
    bool GetNext(Matrix4& value) const noexcept;
    bool GetNext(std::string& value) const noexcept;
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

ArgumentParserState operator|(ArgumentParserState a, const ArgumentParserState& b) noexcept;
ArgumentParserState operator&(ArgumentParserState a, const ArgumentParserState& b) noexcept;
ArgumentParserState& operator|=(const ArgumentParserState& a, const ArgumentParserState& b) noexcept;
ArgumentParserState& operator&=(const ArgumentParserState& a, const ArgumentParserState& b) noexcept;

template<typename T>
ArgumentParser& operator>>(ArgumentParser& parser, T&& arg) noexcept {
    parser.GetNext(std::forward<T>(arg));
    return parser;
}
