#pragma once

#include "Engine/Core/Win.hpp"
#include "Engine/RHI/RHITypes.hpp"

#include <functional>
#include <string>

class IntVector2;

class Window {
public:
    Window() noexcept;
    explicit Window(const IntVector2& position, const IntVector2& dimensions) noexcept;
    ~Window() noexcept;

    void Open() noexcept;
    void Close() noexcept;

    void Show() noexcept;
    void Hide() noexcept;
    void UnHide() noexcept;
    bool IsOpen() const noexcept;
    bool IsClosed() const noexcept;
    bool IsWindowed() const noexcept;
    bool IsFullscreen() const noexcept;

    IntVector2 GetDimensions() const noexcept;
    IntVector2 GetClientDimensions() const noexcept;
    IntVector2 GetPosition() const noexcept;

    static IntVector2 GetDesktopResolution() noexcept;

    void SetDimensionsAndPosition(const IntVector2& new_position, const IntVector2& new_size) noexcept;
    void SetPosition(const IntVector2& new_position) noexcept;
    void SetDimensions(const IntVector2& new_dimensions) noexcept;
    void SetForegroundWindow() noexcept;
    void SetFocus() noexcept;

    void* GetWindowHandle() const noexcept;
    void SetWindowHandle(void* hWnd) noexcept;

    HDC GetWindowDeviceContext() const noexcept;

    const RHIOutputMode& GetDisplayMode() const noexcept;
    void SetDisplayMode(const RHIOutputMode& display_mode) noexcept;

    std::function<bool(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)> custom_message_handler;

    void SetTitle(const std::string& title) noexcept;

protected:
    bool Register() noexcept;
    bool Unregister() noexcept;
    bool Create() noexcept;

private:
    RHIOutputMode _currentDisplayMode = RHIOutputMode::Windowed;
    HWND _hWnd{};
    HDC _hdc{};
    HINSTANCE _hInstance{};
    std::string _title{"DEFAULT WINDOW"};
    INT _cmdShow{};
    WNDCLASSEX _wc{};
    int _positionX{};
    int _positionY{};
    unsigned int _width{1600u};
    unsigned int _height{900u};
    unsigned int _oldclientWidth{1600u};
    unsigned int _oldclientHeight{900u};
    unsigned int _clientWidth{1600u};
    unsigned int _clientHeight{900u};
    unsigned long _styleFlags{};
    unsigned long _styleFlagsEx{};
    bool _hasMenu{};
    static inline std::size_t _refCount{0u};
    static inline constexpr unsigned long _defaultWindowedStyleFlags{WS_CAPTION | WS_SIZEBOX | WS_MINIMIZEBOX | WS_BORDER | WS_SYSMENU | WS_OVERLAPPED};
    static inline constexpr unsigned long _defaultBorderlessStyleFlags{WS_POPUP};
    static inline constexpr unsigned long _defaultStyleFlagsEx{WS_EX_APPWINDOW | WS_EX_ACCEPTFILES};
};
