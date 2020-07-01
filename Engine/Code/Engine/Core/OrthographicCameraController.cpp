#include "Engine/Core/OrthographicCameraController.hpp"

#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include <algorithm>

OrthographicCameraController::OrthographicCameraController(Renderer* renderer, InputSystem* inputSystem, float aspectRatio /*= 1.777778f*/) noexcept
: m_renderer(renderer)
, m_inputSystem(inputSystem)
, m_aspectRatio(aspectRatio)
{
    /* DO NOTHING */
}

void OrthographicCameraController::Update([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {
    if(m_inputSystem->IsKeyDown(KeyCode::RButton)) {
        const auto up = -Vector2::Y_AXIS * m_translationSpeed * deltaSeconds.count();
        const auto down = -up;
        const auto left = -Vector2::X_AXIS * m_translationSpeed * deltaSeconds.count();
        const auto right = -left;
        if(m_inputSystem->IsKeyDown(KeyCode::W)) {
            Translate(up);
        } else if(m_inputSystem->IsKeyDown(KeyCode::S)) {
            Translate(down);
        }
        if(m_inputSystem->IsKeyDown(KeyCode::A)) {
            Translate(left);
        } else if(m_inputSystem->IsKeyDown(KeyCode::D)) {
            Translate(right);
        }
        if(m_inputSystem->WasMouseWheelJustScrolledUp()) {
            ZoomIn();
        } else if(m_inputSystem->WasMouseWheelJustScrolledDown()) {
            ZoomOut();
        }
    }
    m_zoomLevel = std::clamp(m_zoomLevel, m_minZoomLevel, m_maxZoomLevel);
    m_Camera.Update(deltaSeconds);
    m_Camera.SetupView(Vector2{-m_aspectRatio * m_zoomLevel, m_zoomLevel}, Vector2{m_aspectRatio * m_zoomLevel, -m_zoomLevel}, Vector2{0.0f, 1.0f}, m_aspectRatio);
    m_renderer->SetCamera(m_Camera);
}

void OrthographicCameraController::SetAspectRatio(float aspectRatio) noexcept {
    m_aspectRatio = aspectRatio;
}

float OrthographicCameraController::GetAspectRatio() const noexcept {
    return m_aspectRatio;
}

void OrthographicCameraController::SetPosition(const Vector2& newPosition) noexcept {
    m_Camera.SetPosition(newPosition);
}

void OrthographicCameraController::SetRotationDegrees(float newRotation) noexcept {
    m_Camera.SetOrientationDegrees(newRotation);
}

void OrthographicCameraController::SetRotationRadians(float newRotation) noexcept {
    m_Camera.SetOrientation(newRotation);
}

void OrthographicCameraController::ZoomOut() {
    m_zoomLevel += m_zoomSpeed;
}

void OrthographicCameraController::ZoomIn() {
    m_zoomLevel -= m_zoomSpeed;
}

void OrthographicCameraController::Translate(const Vector2& offset) noexcept {
    m_Camera.Translate(offset / m_zoomLevel);
}

void OrthographicCameraController::RotateDegrees(float offset) noexcept {
    m_Camera.ApplyOrientationDegrees(offset);
}

void OrthographicCameraController::RotateRadians(float offset) noexcept {
    m_Camera.ApplyOrientation(offset);
}

void OrthographicCameraController::ResetZoomLevelRange() noexcept {
    SetZoomLevelRange(Vector2{m_defaultMinZoomLevel, m_defaultMaxZoomLevel});
}

void OrthographicCameraController::SetZoomLevel(float zoom) noexcept {
    m_zoomLevel = zoom;
}

void OrthographicCameraController::SetZoomLevelRange(const Vector2& minmaxZoomLevel) noexcept {
    SetMinZoomLevel(minmaxZoomLevel.x);
    SetMaxZoomLevel(minmaxZoomLevel.y);
}

void OrthographicCameraController::SetMinZoomLevel(float minimumLevel) noexcept {
    m_minZoomLevel = (std::max)(1.0f, minimumLevel);
}

void OrthographicCameraController::SetMaxZoomLevel(float maximumValue) noexcept {
    m_maxZoomLevel = maximumValue;
}

const Camera2D& OrthographicCameraController::GetCamera() const noexcept {
    return m_Camera;
}

Camera2D& OrthographicCameraController::GetCamera() noexcept {
    return m_Camera;
}
