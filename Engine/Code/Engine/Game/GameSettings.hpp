#pragma once

class GameSettings {
public:
    [[nodiscard]] float GetWindowWidth() const noexcept;
    [[nodiscard]] float GetWindowHeight() const noexcept;

    [[nodiscard]] float GetVerticalFov() const noexcept;

    void SetWindowWidth(float newWidth) noexcept;
    void SetWindowHeight(float newHeight) noexcept;

    void SetVerticalFov(float newFov) noexcept;

    [[nodiscard]] bool IsMouseInvertedX() const noexcept;
    [[nodiscard]] bool IsMouseInvertedY() const noexcept;
    [[nodiscard]] bool IsVsyncEnabled() const noexcept;

    [[nodiscard]] float DefaultWindowWidth() const noexcept;
    [[nodiscard]] float DefaultWindowHeight() const noexcept;

    [[nodiscard]] float DefaultVerticalFov() const noexcept;

    [[nodiscard]] bool DefaultMouseInvertedX() const noexcept;
    [[nodiscard]] bool DefaultMouseInvertedY() const noexcept;
    [[nodiscard]] bool DefaultVsyncEnabled() const noexcept;

protected:
    float _windowWidth = 1600.0f;
    float _defaultWindowWidth = 1600.0f;
    float _windowHeight = 900.0f;
    float _defaultWindowHeight = 900.0f;
    float _fov = 70.0f;
    float _defaultFov = 70.0f;
    bool _invertMouseY = false;
    bool _defaultInvertMouseY = false;
    bool _invertMouseX = false;
    bool _defaultInvertMouseX = false;
    bool _vsync = false;
    bool _defaultvsync = false;
private:
};
