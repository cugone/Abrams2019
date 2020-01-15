#pragma once

#include "Engine/Core/Win.hpp"

#include <string>

class Clipboard {
public:
    Clipboard() = default;
    explicit Clipboard(HWND hwnd) noexcept;
    Clipboard(const Clipboard& other) = default;
    Clipboard(Clipboard&& other) = default;
    Clipboard& operator=(const Clipboard& other) = default;
    Clipboard& operator=(Clipboard&& other) = default;
    ~Clipboard() noexcept;

    bool Open(HWND hwnd) noexcept;
    bool IsOpen() const noexcept;
    bool IsClosed() const noexcept;
    static bool HasText() noexcept;
    static bool HasUnicodeText() noexcept;
    bool Copy(const std::string& text) noexcept;
    bool Copy(const std::wstring& text) noexcept;
    std::string Paste() noexcept;
    bool Empty() noexcept;
    void Close() noexcept;

protected:
private:
    template<typename T>
    bool Copy_helper(const T& text);

    HWND _hwnd{};
    bool _is_open{ false };
};

template<typename T>
bool Clipboard::Copy_helper(const T& text) {
    bool did_copy = false;
    if(text.empty()) {
        return did_copy;
    }
    if(!HasText()) {
        return did_copy;
    }
    if(Open(_hwnd)) {
        if(Empty()) {
            if(auto hgblcopy = ::GlobalAlloc(GMEM_MOVEABLE, (text.size() + 1) * sizeof(T::value_type))) {
                if(auto lpstrcopy = reinterpret_cast<LPTSTR>(::GlobalLock(hgblcopy))) {
                    std::memcpy(lpstrcopy, text.data(), text.size() + 1);
                    lpstrcopy[text.size() + 1] = '\0';
                }
                ::GlobalUnlock(hgblcopy);
                ::SetClipboardData(CF_TEXT, hgblcopy);
                did_copy = true;
            }
        }
        Close();
    }
    return did_copy;
}
