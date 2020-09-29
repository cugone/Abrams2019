#include "Engine/Core/Clipboard.hpp"

Clipboard::Clipboard(void* hwnd) noexcept
: _hwnd(static_cast<HWND>(hwnd)) {
    Open(_hwnd);
}

Clipboard::~Clipboard() noexcept {
    if(IsOpen()) {
        Close();
    }
}

bool Clipboard::Open(void* hwnd) noexcept {
    _hwnd = static_cast<HWND>(hwnd);
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
    return Copy_helper(text);
}

bool Clipboard::Copy(const std::wstring& text) noexcept {
    return Copy_helper(text);
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
        if(auto hglb = ::GetClipboardData(CF_TEXT)) {
            if(auto lpstrpaste = static_cast<LPTSTR>(::GlobalLock(hglb))) {
                text_to_paste = lpstrpaste;
                ::GlobalUnlock(hglb);
            }
        }
    }
    return text_to_paste;
}
