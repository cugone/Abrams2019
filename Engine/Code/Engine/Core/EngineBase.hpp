#pragma once

#include "Engine/Core/Win.hpp"
#include "Engine/Renderer/Window.hpp"

Window* GetWindowFromHwnd(HWND hwnd);

LRESULT CALLBACK EngineMessageHandlingProcedure(HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam);
