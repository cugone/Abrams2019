#include "Engine/Game/GameSettings.hpp"

IntVector2 GameSettings::GetWindowResolution() const noexcept {
    return IntVector2{GetWindowWidth(), GetWindowHeight()};
}

int GameSettings::GetWindowWidth() const noexcept {
    return _windowWidth;
}

int GameSettings::GetWindowHeight() const noexcept {
    return _windowHeight;
}

float GameSettings::GetVerticalFov() const noexcept {
    return _fov;
}

void GameSettings::SetWindowResolution(const IntVector2& newResolution) noexcept {
    SetWindowWidth(newResolution.x);
    SetWindowHeight(newResolution.y);
}

void GameSettings::SetWindowWidth(int newWidth) noexcept {
    _windowWidth = newWidth;
}

void GameSettings::SetWindowHeight(int newHeight) noexcept {
    _windowHeight = newHeight;
}

void GameSettings::SetVerticalFov(float newFov) noexcept {
    _fov = newFov;
}

bool GameSettings::IsMouseInvertedX() const noexcept {
    return _invertMouseX;
}

bool GameSettings::IsMouseInvertedY() const noexcept {
    return _invertMouseY;
}

bool GameSettings::IsVsyncEnabled() const noexcept {
    return _vsync;
}

int GameSettings::DefaultWindowWidth() const noexcept {
    return _defaultWindowWidth;
}

int GameSettings::DefaultWindowHeight() const noexcept {
    return _defaultWindowHeight;
}

float GameSettings::DefaultVerticalFov() const noexcept {
    return _defaultFov;
}

bool GameSettings::DefaultMouseInvertedX() const noexcept {
    return _defaultInvertMouseX;
}

bool GameSettings::DefaultMouseInvertedY() const noexcept {
    return _defaultInvertMouseY;
}

bool GameSettings::DefaultVsyncEnabled() const noexcept {
    return _defaultvsync;
}
