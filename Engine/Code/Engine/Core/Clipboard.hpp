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
    HWND _hwnd{};
    bool _is_open{ false };
};
