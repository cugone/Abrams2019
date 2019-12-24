#include "Engine/Renderer/Window.hpp"

#include "Engine/Core/EngineBase.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

#include "Engine/Math/IntVector2.hpp"

#include "Engine/RHI/RHITypes.hpp"

#include <algorithm>

Window::Window() noexcept
    : _styleFlags{_defaultBorderlessStyleFlags}
    , _styleFlagsEx{_defaultStyleFlagsEx}
{
    if(_refCount == 0) {
        if(Register()) {
            ++_refCount;
        }
    }

    RECT desktopRect;
    HWND desktopHwnd = ::GetDesktopWindow();
    ::GetClientRect(desktopHwnd, &desktopRect);
    ::AdjustWindowRectEx(&desktopRect, _styleFlags, _hasMenu, _styleFlagsEx);
}

Window::Window(const IntVector2& position, const IntVector2& dimensions) noexcept
    : _styleFlags{_defaultWindowedStyleFlags}
    , _styleFlagsEx{_defaultStyleFlagsEx}
{
    if(_refCount == 0) {
        if(Register()) {
            ++_refCount;
        }
    }
    SetDimensionsAndPosition(position, dimensions);
}

Window::~Window() noexcept {
    Close();
    if(_refCount != 0) {
        --_refCount;
        if(_refCount == 0) {
            Unregister();
        }
    }
}

void Window::Open() noexcept {
    if(IsOpen()) {
        return;
    }
    if(Create()) {
        Show();
        SetForegroundWindow();
        SetFocus();

        HCURSOR cursor = ::LoadCursor(nullptr, IDC_ARROW);
        ::SetCursor(cursor);
    }
}

void Window::Close() noexcept {
    ::DestroyWindow(_hWnd);
}

void Window::Show() noexcept {
    ::ShowWindow(_hWnd, SW_SHOW);
}

void Window::Hide() noexcept {
    ::ShowWindow(_hWnd, SW_HIDE);
}

void Window::UnHide() noexcept {
    Show();
}

bool Window::IsOpen() const noexcept {
    return 0 != ::IsWindow(_hWnd);
}

bool Window::IsClosed() const noexcept {
    return !IsOpen();
}

bool Window::IsWindowed() const noexcept {
    return _currentDisplayMode == RHIOutputMode::Windowed;
}

bool Window::IsFullscreen() const noexcept {
    return _currentDisplayMode == RHIOutputMode::Borderless_Fullscreen;
}

IntVector2 Window::GetDimensions() const noexcept {
    return IntVector2(_width, _height);
}

IntVector2 Window::GetClientDimensions() const noexcept {
    return IntVector2(_clientWidth, _clientHeight);
}

IntVector2 Window::GetPosition() const noexcept {
    return IntVector2(_positionX, _positionY);
}

IntVector2 Window::GetDesktopResolution() noexcept {
    const auto desktop = ::GetDesktopWindow();
    RECT desktop_rect{};
    if(::GetClientRect(desktop, &desktop_rect)) {
        return IntVector2{desktop_rect.right - desktop_rect.left, desktop_rect.bottom - desktop_rect.top};
    } else {
        const auto err = ::GetLastError();
        const auto err_str = StringUtils::FormatWindowsMessage(err);
        ERROR_AND_DIE(err_str.c_str());
    }
}

void Window::SetDimensionsAndPosition(const IntVector2& new_position, const IntVector2& new_size) noexcept {
    RECT r{};
    r.top = static_cast<long>(new_position.y);
    r.left = static_cast<long>(new_position.x);
    r.bottom = r.top + new_size.y;
    r.right = r.left + new_size.x;
    ::SetWindowPos(_hWnd, HWND_TOP, r.left, r.top, r.right - r.left, r.bottom - r.top, SWP_SHOWWINDOW);
    _positionX = r.left;
    _positionY = r.top;
    _width = r.right - r.left;
    _height = r.bottom - r.top;
    _oldclientWidth  = _clientWidth;
    _oldclientHeight = _clientHeight;
    _clientWidth = new_size.x;
    _clientHeight = new_size.y;
}

void Window::SetPosition(const IntVector2& new_position) noexcept {
    SetDimensionsAndPosition(new_position, GetDimensions());
}

void Window::SetDimensions(const IntVector2& new_dimensions) noexcept {
    SetDimensionsAndPosition(GetPosition(), new_dimensions);
}

void Window::SetForegroundWindow() noexcept {
    ::SetForegroundWindow(_hWnd);
}

void Window::SetFocus() noexcept {
    ::SetFocus(_hWnd);
}

HWND Window::GetWindowHandle() const noexcept {
    return _hWnd;
}

void Window::SetWindowHandle(HWND hWnd) noexcept {
    _hWnd = hWnd;
}


HDC Window::GetWindowDeviceContext() const noexcept {
    return _hdc;
}

const RHIOutputMode& Window::GetDisplayMode() const noexcept {
    return _currentDisplayMode;
}

void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
    UNUSED(x);
    UNUSED(y);
    UNUSED(keyFlags);

    static WINDOWPLACEMENT g_wpPrev = {sizeof(g_wpPrev)};
    auto dwStyle = ::GetWindowLongPtr(hwnd, GWL_STYLE);
    if(dwStyle & WS_OVERLAPPEDWINDOW) {
        MONITORINFO mi = {sizeof(mi)};
        if(::GetWindowPlacement(hwnd, &g_wpPrev) &&
            ::GetMonitorInfo(::MonitorFromWindow(hwnd,
                MONITOR_DEFAULTTOPRIMARY), &mi)) {
            ::SetWindowLongPtr(hwnd, GWL_STYLE,
                dwStyle & ~WS_OVERLAPPEDWINDOW);
            ::SetWindowPos(hwnd, HWND_TOP,
                mi.rcMonitor.left, mi.rcMonitor.top,
                mi.rcMonitor.right - mi.rcMonitor.left,
                mi.rcMonitor.bottom - mi.rcMonitor.top,
                SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        }
    } else {
        ::SetWindowLongPtr(hwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
        ::SetWindowPlacement(hwnd, &g_wpPrev);
        ::SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
            SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
}

void Window::SetDisplayMode(const RHIOutputMode& display_mode) noexcept {
    if(display_mode == _currentDisplayMode) {
        return;
    }

    static WINDOWPLACEMENT g_wpPrev = {sizeof(g_wpPrev)};
    auto dwStyle = ::GetWindowLongPtr(_hWnd, GWL_STYLE);

    _currentDisplayMode = display_mode;
    switch(_currentDisplayMode) {
        case RHIOutputMode::Windowed:
        {
            ::SetWindowLongPtr(_hWnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
            ::SetWindowPlacement(_hWnd, &g_wpPrev);
            ::SetWindowPos(_hWnd, nullptr, 0, 0, 0, 0,
                SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

            //_styleFlags |= _defaultWindowedStyleFlags;
            //::SetWindowLongPtr(_hWnd, GWL_STYLE, _styleFlags);
            //RECT r{_positionX, _positionY, static_cast<long>(_positionX + _clientWidth), static_cast<long>(_positionY + _clientHeight)};

            //RECT desktopRect;
            //HWND desktopWindowHandle = ::GetDesktopWindow();
            //::GetClientRect(desktopWindowHandle, &desktopRect);
            //const int x = static_cast<int>(desktopRect.right - desktopRect.left) / 2 - static_cast<int>(_clientWidth) / 2;
            //const int y = static_cast<int>(desktopRect.bottom - desktopRect.top) / 2 + static_cast<int>(_clientHeight) / 2;
            //const int w = _clientWidth;
            //const int h = _clientHeight;
            //SetDimensionsAndPosition(IntVector2{x, y}, IntVector2{w, h});
            break;
        }
        case RHIOutputMode::Borderless_Fullscreen:
        {
            MONITORINFO mi = {sizeof(mi)};
            if(::GetWindowPlacement(_hWnd, &g_wpPrev) &&
                ::GetMonitorInfo(::MonitorFromWindow(_hWnd,
                    MONITOR_DEFAULTTOPRIMARY), &mi)) {
                ::SetWindowLongPtr(_hWnd, GWL_STYLE,
                    dwStyle & ~WS_OVERLAPPEDWINDOW);
                ::SetWindowPos(_hWnd, HWND_TOP,
                    mi.rcMonitor.left, mi.rcMonitor.top,
                    mi.rcMonitor.right - mi.rcMonitor.left,
                    mi.rcMonitor.bottom - mi.rcMonitor.top,
                    SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
            }

            //_styleFlags = WS_POPUP;
            //::SetWindowLongPtr(_hWnd, GWL_STYLE, _styleFlags);
            //RECT desktopRect;
            //HWND desktopWindowHandle = GetDesktopWindow();
            //::GetClientRect(desktopWindowHandle, &desktopRect);

            //long width = desktopRect.right - desktopRect.left;
            //long height = desktopRect.bottom - desktopRect.top;
            //SetDimensionsAndPosition(IntVector2::ZERO, IntVector2(width, height));
            break;
        }
        default:
            /* DO NOTHING */;
    }
    Show();
}

void Window::SetTitle(const std::string& title) noexcept {
    _title = title;
    ::SetWindowTextA(_hWnd, _title.data());
}

bool Window::Register() noexcept {
    _hInstance = GetModuleHandle(nullptr);
    memset(&_wc, 0, sizeof(_wc));
    auto window_class_name = "Simple Window Class";
    _wc.cbSize = sizeof(_wc);
    _wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    _wc.lpfnWndProc = EngineMessageHandlingProcedure;
    _wc.cbClsExtra = 0;
    _wc.cbWndExtra = 0;
    _wc.hInstance = _hInstance;
    _wc.hIcon = nullptr;
    _wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    _wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    _wc.lpszMenuName = NULL;
    _wc.lpszClassName = window_class_name;
    _wc.hIconSm = nullptr;
    return 0 != RegisterClassEx(&_wc);
}

bool Window::Unregister() noexcept {
    return 0 != ::UnregisterClass(_wc.lpszClassName, nullptr);
}

bool Window::Create() noexcept {
    _hWnd = ::CreateWindowEx(
        _styleFlagsEx,                              // Optional window styles.
        _wc.lpszClassName,              // Window class
        "Created with Abrams 2019 (c) Casey Ugone",     // Window text
        _styleFlags,            // Window style
        _positionX, _positionY,                           //Position XY
        _width, _height,    //Size WH
        nullptr,       // Parent window    
        nullptr,       // Menu
        _hInstance,     // Instance handle
        this        // Additional application data
    );
    _hdc = ::GetDCEx(_hWnd, nullptr, 0);

    return _hWnd != nullptr;
}

