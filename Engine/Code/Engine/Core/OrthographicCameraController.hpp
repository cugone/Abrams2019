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

    void SetAspectRatio(float aspectRatio) noexcept;
    float GetAspectRatio() const noexcept;

    void SetPosition(const Vector2& newPosition) noexcept;
    void Translate(const Vector2& offset) noexcept;
    
    void SetRotationDegrees(float newRotation) noexcept;
    void SetRotationRadians(float newRotation) noexcept;
    void RotateDegrees(float offset) noexcept;
    void RotateRadians(float offset) noexcept;

    void ZoomIn();
    void ZoomOut();

    void ResetZoomLevelRange() noexcept;
    void SetZoomLevel(float zoom) noexcept;
    void SetZoomLevelRange(const Vector2& minmaxZoomLevel) noexcept;
    void SetMinZoomLevel(float minimumLevel) noexcept;
    void SetMaxZoomLevel(float maximumValue) noexcept;

    const Camera2D& GetCamera() const noexcept;
    Camera2D& GetCamera() noexcept;

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
    Camera2D m_Camera{};
    float m_translationSpeed = 5.0f;
    float m_rotationSpeed = 180.0f;
    float m_zoomSpeed = 8.0f;
    float m_maxZoomSpeed = 24.0f;
};
