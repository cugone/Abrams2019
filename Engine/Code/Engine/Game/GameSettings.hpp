#pragma once

class GameSettings {
public:
    [[nodiscard]] float GetWindowWidth() const noexcept;
    [[nodiscard]] float GetWindowHeight() const noexcept;
    [[nodiscard]] int GetWindowWidth() const noexcept;
    [[nodiscard]] int GetWindowHeight() const noexcept;

    [[nodiscard]] float GetVerticalFov() const noexcept;

    void SetWindowWidth(float newWidth) noexcept;
    void SetWindowWidth(int newWidth) noexcept;
    void SetWindowHeight(int newHeight) noexcept;

    void SetVerticalFov(float newFov) noexcept;

    [[nodiscard]] bool IsMouseInvertedX() const noexcept;
    [[nodiscard]] bool IsMouseInvertedY() const noexcept;
    [[nodiscard]] bool IsVsyncEnabled() const noexcept;

    [[nodiscard]] int DefaultWindowWidth() const noexcept;
    [[nodiscard]] int DefaultWindowHeight() const noexcept;

    [[nodiscard]] float DefaultVerticalFov() const noexcept;

    [[nodiscard]] bool DefaultMouseInvertedX() const noexcept;
    [[nodiscard]] bool DefaultMouseInvertedY() const noexcept;
    [[nodiscard]] bool DefaultVsyncEnabled() const noexcept;

protected:
    int _windowWidth = 1600;
    int _defaultWindowWidth = 1600;
    int _windowHeight = 900;
    int _defaultWindowHeight = 900;
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
