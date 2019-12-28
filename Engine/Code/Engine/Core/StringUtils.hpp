#pragma once

#include <string>
#include <string_view>
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

std::string FormatWindowsMessage(unsigned long messageId) noexcept;
std::string FormatWindowsLastErrorMessage() noexcept;

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
std::string ROT13(std::string_view text) noexcept;

//NOT USEFUL AS TRUE ENCRYPTION!! DO NOT USE IF SERIOUS ENCRYPTION IS NEEDED!!!
std::string CaesarShift(std::string_view text, bool encode = true) noexcept;

//NOT USEFUL AS TRUE ENCRYPTION!! DO NOT USE IF SERIOUS ENCRYPTION IS NEEDED!!!
std::string ShiftCipher(int key, std::string_view text) noexcept;

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

} //End detail


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
