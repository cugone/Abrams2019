#include "Engine/Renderer/Window.hpp"

#include "Engine/Core/EngineBase.hpp"

#include "Engine/Math/IntVector2.hpp"

#include "Engine/RHI/RHITypes.hpp"

std::size_t Window::_refCount{};

Window::Window() noexcept {
    if(_refCount == 0) {
        if(Register()) {
            ++_refCount;
        }
    }

    RECT desktopRect;
    HWND desktopHwnd = ::GetDesktopWindow();
    ::GetClientRect(desktopHwnd, &desktopRect);

    ::AdjustWindowRectEx(&desktopRect, _styleFlags, _hasMenu, _styleFlagsEx);
    ::GetClipCursor(&_initialClippingArea);

}

Window::Window(const IntVector2& position, const IntVector2& dimensions) noexcept {
    if(_refCount == 0) {
        if(Register()) {
            ++_refCount;
        }
    }

    RECT desktopRect;
    HWND desktopHwnd = ::GetDesktopWindow();
    ::GetClientRect(desktopHwnd, &desktopRect);

    RECT r;
    r.left = position.x;
    r.top = position.y;
    r.right = r.left + dimensions.x;
    r.bottom = r.top + dimensions.y;
    ::AdjustWindowRectEx(&r, _styleFlags, _hasMenu, _styleFlagsEx);

    ::GetClipCursor(&_initialClippingArea);
}

Window::~Window() noexcept {
    ::ClipCursor(&_initialClippingArea);
    Close();
    if(_refCount != 0) {
        --_refCount;
        if(_refCount == 0) {
            Unregister();
        }
    }
}

void Window::Open() noexcept {
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
    return true;
}

bool Window::IsFullscreen() const noexcept {
    return !IsWindowed();
}

IntVector2 Window::GetDimensions() const noexcept {
    return IntVector2(_width, _height);
}

IntVector2 Window::GetPosition() const noexcept {
    return IntVector2(_positionX, _positionY);
}

void Window::SetDimensionsAndPosition(const IntVector2& new_position, const IntVector2& new_size) noexcept {
    RECT r;
    r.top = new_position.y;
    r.left = new_position.x;
    r.bottom = r.top + new_size.y;
    r.right = r.left + new_size.x;
    ::AdjustWindowRectEx(&r, _styleFlags, _hasMenu, _styleFlagsEx);
    _width = r.right - r.left;
    _height = r.bottom - r.top;
    _positionX = r.left;
    _positionY = r.top;
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

const RHIOutputMode& Window::GetDisplayMode() const noexcept {
    return _currentDisplayMode;
}

void Window::SetDisplayMode(const RHIOutputMode& display_mode) noexcept {
    if(display_mode == _currentDisplayMode) {
        return;
    }
    _currentDisplayMode = display_mode;
    RECT r;
    r.top = _positionY;
    r.left = _positionX;
    r.bottom = r.top + _height;
    r.right = r.left + _width;
    switch(_currentDisplayMode) {
        case RHIOutputMode::Borderless:
        {
            _styleFlags = WS_POPUP;
            _hasMenu = false;

            ::GetClientRect(_hWnd, &r);

            long width = r.right - r.left;
            long height = r.bottom - r.top;
            ::SetWindowLongPtr(_hWnd, GWL_STYLE, _styleFlags);
            ::SetWindowPos(_hWnd, nullptr, 0, 0, width, height, SWP_SHOWWINDOW);
            SetDimensionsAndPosition(IntVector2::ZERO, IntVector2(width, height));
            ::AdjustWindowRectEx(&r, _styleFlags, _hasMenu, _styleFlagsEx);
            return;
        } case RHIOutputMode::Windowed:
        {
            _styleFlags = WS_CAPTION | WS_BORDER | WS_SYSMENU | WS_OVERLAPPED;
            break;
        } case RHIOutputMode::Fullscreen_Window:
        {

            _styleFlags = WS_POPUP;

            RECT desktopRect;
            HWND desktopWindowHandle = GetDesktopWindow();
            ::GetClientRect(desktopWindowHandle, &desktopRect);

            long width = desktopRect.right - desktopRect.left;
            long height = desktopRect.bottom - desktopRect.top;
            ::SetWindowLongPtr(_hWnd, GWL_STYLE, _styleFlags);
            ::SetWindowPos(_hWnd, nullptr, 0, 0, width, height, SWP_SHOWWINDOW);
            SetDimensionsAndPosition(IntVector2::ZERO, IntVector2(width, height));
            ::AdjustWindowRectEx(&r, _styleFlags, _hasMenu, _styleFlagsEx);
            return;
        }
        default:
            /* DO NOTHING */;
    }
    ::SetWindowLongPtr(_hWnd, GWL_STYLE, _styleFlags);
    ::AdjustWindowRectEx(&r, _styleFlags, _hasMenu, _styleFlagsEx);

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
    _styleFlags = WS_CAPTION | WS_BORDER | WS_SYSMENU | WS_OVERLAPPED;
    _styleFlagsEx = WS_EX_APPWINDOW;
    _hWnd = ::CreateWindowEx(
        _styleFlagsEx,                              // Optional window styles.
        _wc.lpszClassName,              // Window class
        "Learn to Program Windows",     // Window text
        _styleFlags,            // Window style
        _positionX, _positionY,                           //Position XY
        _width, _height,    //Size WH
        nullptr,       // Parent window    
        nullptr,       // Menu
        _hInstance,     // Instance handle
        this        // Additional application data
    );
    return _hWnd != nullptr;
}

