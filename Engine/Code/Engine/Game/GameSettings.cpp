#include "Engine/Game/GameSettings.hpp"

float GameSettings::GetWindowWidth() const noexcept {
    return _windowWidth;
}

float GameSettings::GetWindowHeight() const noexcept {
    return _windowHeight;
}

float GameSettings::GetVerticalFov() const noexcept {
    return _fov;
}

void GameSettings::SetWindowWidth(float newWidth) noexcept {
    _windowWidth = newWidth;
}

void GameSettings::SetWindowHeight(float newHeight) noexcept {
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

float GameSettings::DefaultWindowWidth() const noexcept {
    return _defaultWindowWidth;
}

float GameSettings::DefaultWindowHeight() const noexcept {
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
