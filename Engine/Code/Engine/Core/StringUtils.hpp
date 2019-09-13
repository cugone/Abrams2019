#pragma once

#include <string>
#include <tuple>
#include <utility>
#include <vector>


class Vector2;
class Vector3;
class Vector4;
class Matrix4;
class Rgba;
namespace System {
    struct SystemDesc;
}
namespace System::Cpu {
    enum class ProcessorArchitecture;
}

namespace StringUtils {

std::string to_string(const Vector2& v) noexcept;
std::string to_string(const Vector3& v) noexcept;
std::string to_string(const Vector4& v) noexcept;
std::string to_string(const Matrix4& m) noexcept;
std::string to_string(const Rgba& clr) noexcept;
std::string to_string(const System::Cpu::ProcessorArchitecture& architecture) noexcept;
std::string to_string(const System::SystemDesc& system) noexcept;

const std::string Stringf(const char* format, ...) noexcept;
const std::string Stringf(const int maxLength, const char* format, ...) noexcept;

std::vector<std::string> Split(const std::string& string, char delim = ',', bool skip_empty = true) noexcept;
std::vector<std::wstring> Split(const std::wstring& string, wchar_t delim = ',', bool skip_empty = true) noexcept;
std::vector<std::string> SplitOnUnquoted(const std::string& string, char delim = ',', bool skip_empty = true) noexcept;
std::vector<std::wstring> SplitOnUnquoted(const std::wstring& string, wchar_t delim = ',', bool skip_empty = true) noexcept;
std::pair<std::string, std::string> SplitOnFirst(const std::string& string, char delim) noexcept;
std::pair<std::wstring, std::wstring> SplitOnFirst(const std::wstring& string, wchar_t delim) noexcept;
std::pair<std::string, std::string> SplitOnLast(const std::string& string, char delim) noexcept;
std::pair<std::wstring, std::wstring> SplitOnLast(const std::wstring& string, wchar_t delim) noexcept;

std::string Join(const std::vector<std::string>& strings, char delim, bool skip_empty = true) noexcept;
std::wstring Join(const std::vector<std::wstring>& strings, wchar_t delim, bool skip_empty = true) noexcept;
std::string Join(const std::vector<std::string>& strings, bool skip_empty = true) noexcept;
std::wstring Join(const std::vector<std::wstring>& strings, bool skip_empty = true) noexcept;

template<typename T, typename... U>
T Join(char delim, const T& arg, const U& ... args) noexcept {
    return detail::Join(delim, arg, args ...);
}

template<typename T, typename... U>
T JoinSkipEmpty(char delim, const T& arg, const U& ... args) noexcept {
    return detail::JoinSkipEmpty(delim, arg, args ...);
}

template<typename T, typename... U>
T Join(wchar_t delim, const T& arg, const U& ... args) noexcept {
    return detail::Join(delim, arg, args ...);
}

template<typename T, typename... U>
T JoinSkipEmpty(wchar_t delim, const T& arg, const U& ... args) noexcept {
    return detail::JoinSkipEmpty(delim, arg, args ...);
}

template<typename T, typename... U>
T Join(const T& arg, const U& ... args) noexcept {
    return detail::Join(arg, args ...);
}

template<typename T, typename... U>
T JoinSkipEmpty(const T& arg, const U& ... args) noexcept {
    return detail::JoinSkipEmpty(arg, args ...);
}

std::string ToUpperCase(std::string string) noexcept;
std::wstring ToUpperCase(std::wstring string) noexcept;
std::string ToLowerCase(std::string string) noexcept;
std::wstring ToLowerCase(std::wstring string) noexcept;

std::string ConvertUnicodeToMultiByte(const std::wstring& unicode_string) noexcept;
std::wstring ConvertMultiByteToUnicode(const std::string& multi_byte_string) noexcept;

bool StartsWith(const std::string& string, const std::string& start) noexcept;
bool StartsWith(const std::wstring& string, const std::wstring& start) noexcept;
bool StartsWith(const std::string& string, char start) noexcept;
bool StartsWith(const std::wstring& string, wchar_t start) noexcept;

bool EndsWith(const std::string& string, const std::string& end) noexcept;
bool EndsWith(const std::wstring& string, const std::wstring& end) noexcept;
bool EndsWith(const std::string& string, char end) noexcept;
bool EndsWith(const std::wstring& string, wchar_t end) noexcept;

std::string ReplaceAll(std::string string, const std::string& from, const std::string& to) noexcept;
std::wstring ReplaceAll(std::wstring string, const std::wstring& from, const std::wstring& to) noexcept;

std::vector<std::size_t> FindAll(std::string string, const char c) noexcept;
std::vector<std::size_t> FindAll(std::wstring string, const wchar_t c) noexcept;
std::vector<std::size_t> FindAll(std::string string, const std::string& sequence) noexcept;
std::vector<std::size_t> FindAll(std::wstring string, const std::wstring& sequence) noexcept;

std::string TrimWhitespace(std::string string) noexcept;
std::wstring TrimWhitespace(std::wstring string) noexcept;

constexpr const uint32_t FourCC(const char* id) noexcept {
    return static_cast<uint32_t>((((id[0] << 24) & 0xFF000000) | ((id[1] << 16) & 0x00FF0000) | ((id[2] << 8) & 0x0000FF00) | ((id[3] << 0) & 0x000000FF)));
}

void CopyFourCC(char* destFCC, const char* srcFCC) noexcept;
std::string FourCCToString(const char* id) noexcept;

namespace Encryption {

//NOT USEFUL AS TRUE ENCRYPTION!! DO NOT USE IF SERIOUS ENCRYPTION IS NEEDED!!!
std::string ROT13(std::string text) noexcept;

//NOT USEFUL AS TRUE ENCRYPTION!! DO NOT USE IF SERIOUS ENCRYPTION IS NEEDED!!!
std::string CaesarShiftEncode(int key, std::string plaintext) noexcept;

//NOT USEFUL AS TRUE ENCRYPTION!! DO NOT USE IF SERIOUS ENCRYPTION IS NEEDED!!!
std::string CaesarShiftDecode(int key, std::string ciphertext) noexcept;

} //End Encryption

} //End StringUtils


namespace detail {

    template<typename First, typename... Rest>
    First Join(const First& first) noexcept {
        return first;
    }

    template<typename First, typename... Rest>
    First Join(const First& first, const Rest& ... rest) noexcept {
        return first + detail::Join(rest...);
    }

    template<typename First, typename... Rest>
    First Join([[maybe_unused]]char delim, const First& first) noexcept {
        return first;
    }

    template<typename First, typename... Rest>
    First Join(char delim, const First& first, const Rest& ... rest) noexcept {
        return first + First{ delim } + detail::Join(delim, rest...);
    }

    template<typename First, typename... Rest>
    First Join([[maybe_unused]]wchar_t delim, const First& first) noexcept {
        return first;
    }

    template<typename First, typename... Rest>
    First Join(wchar_t delim, const First& first, const Rest& ... rest) noexcept {
        return first + First{ delim } + detail::Join(delim, rest...);
    }

    template<typename First, typename... Rest>
    First JoinSkipEmpty(const First& first) noexcept {
        return first;
    }

    template<typename First, typename... Rest>
    First JoinSkipEmpty(const First& first, const Rest& ... rest) noexcept {
        return first + detail::JoinSkipEmpty(rest...);
    }

    template<typename First, typename... Rest>
    First JoinSkipEmpty(char delim, const First& first) noexcept {
        return first;
    }

    template<typename First, typename... Rest>
    First JoinSkipEmpty(char delim, const First& first, const Rest& ... rest) noexcept {
        if (first.empty()) {
            return detail::JoinSkipEmpty(delim, rest...);
        }
        if(sizeof...(rest) == 1) {
            auto t = std::make_tuple(rest...);
            auto last = std::get<0>(t);
            if(last.empty()) {
                return first;
            }
        }
        return first + First{delim} + detail::JoinSkipEmpty(delim, rest...);
    }
    
    template<typename First, typename... Rest>
    First JoinSkipEmpty([maybe_unused]wchar_t delim) noexcept {
        return First{};
    }

    template<typename First, typename... Rest>
    First JoinSkipEmpty(wchar_t delim, const First& first) noexcept {
        return first;
    }

    template<typename First, typename... Rest>
    First JoinSkipEmpty(wchar_t delim, const First& first, const Rest& ... rest) noexcept {
        if (first.empty()) {
            return detail::JoinSkipEmpty(delim, rest...);
        }
        if (sizeof...(rest) == 1) {
            auto t = std::make_tuple(rest...);
            auto last = std::get<0>(t);
            if (last.empty()) {
                return first;
            }
        }
        return first + First{ delim } +detail::JoinSkipEmpty(delim, rest...);
    }

    struct encode_tag {};
    struct decode_tag {};

    //NOT USEFUL AS TRUE ENCRYPTION!! DO NOT USE IF SERIOUS ENCRYPTION IS NEEDED!!!
    template<int key, typename Op = encode_tag>
    std::string CaesarShift(std::string plaintext) noexcept {
        auto caesarshift = [](unsigned char a) {
            bool lower = 'a' <= a && a <= 'z';
            bool upper = 'A' <= a && a <= 'Z';
            char base = lower ? 'a' : upper ? 'A' : 0;
            if (!base) {
                return a;
            }
            int shift_result = 0;
            if (std::is_same_v<Op, encode_tag>) {
                shift_result = (a - base + key) % 26;
            }
            else if (std::is_same_v<Op, decode_tag>) {
                shift_result = (a - base - key) % 26;
            }
            if (shift_result < 0) {
                shift_result += 26;
            }
            if (25 < shift_result) {
                shift_result -= 26;
            }
            return static_cast<unsigned char>(static_cast<char>(base + shift_result));
        };
        std::transform(std::begin(plaintext), std::end(plaintext), std::begin(plaintext), caesarshift);
        return plaintext;
    }


    //NOT USEFUL AS TRUE ENCRYPTION!! DO NOT USE IF SERIOUS ENCRYPTION IS NEEDED!!!
    template<int key>
    std::string CaesarShiftEncode(std::string plaintext) noexcept {
        return detail::CaesarShift<key, detail::encode_tag>(plaintext);
    }

    //NOT USEFUL AS TRUE ENCRYPTION!! DO NOT USE IF SERIOUS ENCRYPTION IS NEEDED!!!
    template<int key>
    std::string CaesarShiftDecode(std::string ciphertext) noexcept {
        return detail::CaesarShift<key, detail::decode_tag>(ciphertext);
    }
} //End detail
