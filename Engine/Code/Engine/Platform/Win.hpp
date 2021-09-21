#pragma once

#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Math/AABB2.hpp"

#if defined(PLATFORM_WINDOWS)

    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
    #include <windows.h>
    #include <windowsx.h>

    #ifdef NETWORKING
        #include <WS2tcpip.h>
        #include <WinSock2.h>

        #pragma comment(lib, "ws2_32.lib")
    #endif

AABB2 RectToAABB2(const RECT& rect) noexcept;
RECT AABB2ToRect(const AABB2& aabb2) noexcept;

#endif
