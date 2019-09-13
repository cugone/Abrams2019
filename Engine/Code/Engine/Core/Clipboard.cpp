#include "Engine/Core/Clipboard.hpp"

Clipboard::Clipboard(HWND hwnd) noexcept
    : _hwnd(hwnd)
{
    Open(_hwnd);
}

Clipboard::~Clipboard() noexcept {
    if(IsOpen()) {
        Close();
    }
}

bool Clipboard::Open(HWND hwnd) noexcept {
    _hwnd = hwnd;
    _is_open = !!::OpenClipboard(_hwnd);
    return _is_open;
}

bool Clipboard::IsOpen() const noexcept {
    return _is_open;
}

bool Clipboard::IsClosed() const noexcept {
    return !IsOpen();
}

/* static */
bool Clipboard::HasText() noexcept {
    return ::IsClipboardFormatAvailable(CF_TEXT);
}

/* static */
bool Clipboard::HasUnicodeText() noexcept {
    return ::IsClipboardFormatAvailable(CF_UNICODETEXT);
}

bool Clipboard::Copy(const std::string& text) noexcept {
    bool did_copy = false;
    if(text.empty()) {
        return did_copy;
    }
    if(!HasText()) {
        return did_copy;
    }
    if(Open(_hwnd)) {
        if(Empty()) {
            if(auto hgblcopy = ::GlobalAlloc(GMEM_MOVEABLE, (text.size() + 1) * sizeof(std::string::value_type))) {
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

bool Clipboard::Copy(const std::wstring& text) noexcept {
    bool did_copy = false;
    if(text.empty()) {
        return did_copy;
    }
    if(!HasText()) {
        return did_copy;
    }
    if(Open(_hwnd)) {
        if(Empty()) {
            if(auto hgblcopy = ::GlobalAlloc(GMEM_MOVEABLE, (text.size() + 1) * sizeof(std::wstring::value_type))) {
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

bool Clipboard::Empty() noexcept {
    if(IsOpen()) {
        return ::EmptyClipboard();
    }
    return false;
}

void Clipboard::Close() noexcept {
    if(IsOpen()) {
        _is_open = !!!::CloseClipboard();
    }
}

std::string Clipboard::Paste() noexcept {
    std::string text_to_paste{};
    if(HasText()) {
        if(HGLOBAL hglb = ::GetClipboardData(CF_TEXT)) {
            if(auto lpstrpaste = reinterpret_cast<LPTSTR>(::GlobalLock(hglb))) {
                text_to_paste = lpstrpaste;
                ::GlobalUnlock(hglb);
            }
        }
    }
    return text_to_paste;
}
