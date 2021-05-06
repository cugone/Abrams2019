#include "Engine/Core/EngineBase.hpp"

namespace a2de {

    Window* GetWindowFromHwnd(HWND hwnd) {
        Window* wnd = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        return wnd;
    }

    //-----------------------------------------------------------------------------------------------
    LRESULT CALLBACK EngineMessageHandlingProcedure(HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam) {
        //Handles application-specific window setup such as icons.
        Window* window = GetWindowFromHwnd(windowHandle);
        if(window && window->custom_message_handler) {
            const auto wasProcessed = window->custom_message_handler(windowHandle, wmMessageCode, wParam, lParam);
            if(wasProcessed) {
                return 0;
            }
        }

        switch(wmMessageCode) {
        case WM_CREATE: {
            CREATESTRUCT* cp = (CREATESTRUCT*)lParam;
            Window* wnd = (Window*)cp->lpCreateParams;
            ::SetWindowLongPtr(windowHandle, GWLP_USERDATA, (LONG_PTR)wnd);
            return 0;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            ::BeginPaint(windowHandle, &ps);
            ::EndPaint(windowHandle, &ps);
            return 1;
        }
        default: {
            return DefWindowProc(windowHandle, wmMessageCode, wParam, lParam);
        }
        }
    }

} // namespace a2de
