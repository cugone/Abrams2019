#pragma once

#include "Engine/Core/TimeUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Camera2D.hpp"

class Renderer;
class InputSystem;

class OrthographicCameraController {
public:
    OrthographicCameraController() noexcept = default;
    explicit OrthographicCameraController(Renderer* renderer, InputSystem* inputSystem, float aspectRatio = 1.777778f) noexcept;

    void Update([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept;

    void SetupCameraShake(float maxShakeOffsetHorizontal, float maxShakeOffsetVertical, float maxShakeAngleDegrees);

    void SetAspectRatio(float aspectRatio) noexcept;
    [[nodiscard]] float GetAspectRatio() const noexcept;

    void SetPosition(const Vector2& newPosition) noexcept;
    void Translate(const Vector2& offset) noexcept;
    void TranslateTo(const Vector2& position, TimeUtils::FPSeconds t) noexcept;
    void SetRotationDegrees(float newRotation) noexcept;
    void SetRotationRadians(float newRotation) noexcept;
    void RotateDegrees(float offset) noexcept;
    void RotateRadians(float offset) noexcept;

    void ZoomIn();
    void ZoomOut();

    void ResetZoomLevelRange() noexcept;
    float GetZoomLevel() const noexcept;
    float GetZoomRatio() const noexcept;
    void SetZoomLevel(float zoom) noexcept;
    void SetZoomLevelRange(const Vector2& minmaxZoomLevel) noexcept;
    void SetMinZoomLevel(float minimumLevel) noexcept;
    void SetMaxZoomLevel(float maximumValue) noexcept;

    [[nodiscard]] const Camera2D& GetCamera() const noexcept;
    [[nodiscard]] Camera2D& GetCamera() noexcept;

    [[nodiscard]] float GetShake() const noexcept;

    template<typename F>
    void DoCameraShake(F&& f) {
        m_Camera.trauma = f();
    }

protected:
private:
    Renderer* m_renderer{};
    InputSystem* m_inputSystem{};
    float m_aspectRatio = MathUtils::M_16_BY_9_RATIO;
    float m_zoomLevel = 8.0f;
    float m_defaultMinZoomLevel = 8.0f;
    float m_defaultMaxZoomLevel = (std::numeric_limits<float>::max)();
    float m_minZoomLevel = 8.0f;
    float m_maxZoomLevel = (std::numeric_limits<float>::max)();
    mutable Camera2D m_Camera{};
    Camera2D m_ShakyCamera{};
    float m_maxShakeOffsetHorizontal{10.0f};
    float m_maxShakeOffsetVertical{10.0f};
    float m_maxShakeAngle{25.0f};
    float m_translationSpeed = 5.0f;
    float m_rotationSpeed = 180.0f;
    float m_zoomSpeed = 8.0f;
    float m_maxZoomSpeed = 24.0f;
};
