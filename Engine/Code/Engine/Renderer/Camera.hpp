#pragma once

#include "Engine/Core/TimeUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Matrix4.hpp"
#include "Engine/Math/Quaternion.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Renderer/RenderTargetStack.hpp"

enum class ProjectionMode {
    Orthographic,
    Perspective
};

class Camera {
public:

    void SetProjectionMode(ProjectionMode newProjectionMode) noexcept;
    [[nodiscard]] ProjectionMode GetProjectionMode() const noexcept;

    void Update(TimeUtils::FPSeconds deltaSeconds) noexcept;

    [[nodiscard]] const Vector3& GetPosition() const noexcept;
    void SetPosition(const Vector3& newPosition) noexcept;
    void SetPosition(const Vector2& newPosition) noexcept;

    void Translate(const Vector3& displacement) noexcept;
    void Translate(const Vector2& displacement) noexcept;

    void SetOffsets(const Matrix4& transform, float fov) noexcept;
    void AddOffsets(const Matrix4& transform, float fov) noexcept;
    void ClearOffsets() noexcept;

    [[nodiscard]] float GetAspectRatio() const noexcept;
    [[nodiscard]] float GetInverseAspectRatio() const noexcept;
    [[nodiscard]] float GetNearDistance() const noexcept;
    [[nodiscard]] float GetFarDistance() const noexcept;

    [[nodiscard]] const Matrix4& GetViewMatrix() const noexcept;
    [[nodiscard]] const Matrix4& GetProjectionMatrix() const noexcept;
    [[nodiscard]] const Matrix4& GetViewProjectionMatrix() const noexcept;

    [[nodiscard]] const Matrix4& GetInverseViewMatrix() const noexcept;
    [[nodiscard]] const Matrix4& GetInverseProjectionMatrix() const noexcept;
    [[nodiscard]] const Matrix4& GetInverseViewProjectionMatrix() const noexcept;

    [[nodiscard]] const RenderTargetStack::Node& GetRenderTarget() const noexcept;
    [[nodiscard]] RenderTargetStack::Node& GetRenderTarget() noexcept;

    [[nodiscard]] float GetShake() const noexcept;

    [[nodiscard]] const Matrix4& GetRotationMatrix() const noexcept;
    [[nodiscard]] Matrix4 CreateBillboardMatrix(const Matrix4& rotationMatrix) noexcept;
    [[nodiscard]] Matrix4 CreateReverseBillboardMatrix(const Matrix4& rotationMatrix) noexcept;

    [[nodiscard]] Vector3 GetEulerAngles() const noexcept;
    [[nodiscard]] Vector3 GetEulerAnglesDegrees() const noexcept;
    [[nodiscard]] void SetEulerAnglesDegrees(const Vector3& eulerAnglesDegrees) noexcept;
    [[nodiscard]] void SetEulerAngles(const Vector3& eulerAngles) noexcept;
    [[nodiscard]] void SetForwardFromTarget(const Vector3& lookAtPosition) noexcept;

    [[nodiscard]] Vector3 GetRight() const noexcept;
    [[nodiscard]] Vector3 GetUp() const noexcept;
    [[nodiscard]] Vector3 GetForward() const noexcept;

    [[nodiscard]] float GetYawDegrees() const noexcept;
    [[nodiscard]] float GetPitchDegrees() const noexcept;
    [[nodiscard]] float GetRollDegrees() const noexcept;

    [[nodiscard]] float GetYaw() const noexcept;
    [[nodiscard]] float GetPitch() const noexcept;
    [[nodiscard]] float GetRoll() const noexcept;

    float trauma = 0.0f;
    float trauma_recovery_rate = 1.0f;

protected:
private:
    float aspect_ratio = MathUtils::M_16_BY_9_RATIO;
    float fovH = 60.0f;
    float fovOffset = 0.0f;
    float orthoWidth = 8.0f;
    float near_distance = 0.01f;
    float far_distance = 1.0f;
    Vector3 position = Vector3::ZERO;
    Vector3 positionOffset = Vector3::ZERO;
    Vector3 world_up = Vector3::Y_AXIS;

    Matrix4 view_matrix = Matrix4::I;
    Matrix4 rotation_matrix = Matrix4::I;
    Matrix4 projection_matrix = Matrix4::I;
    Matrix4 view_projection_matrix = Matrix4::I;
    Matrix4 inv_view_matrix = Matrix4::I;
    Matrix4 inv_projection_matrix = Matrix4::I;
    Matrix4 inv_view_projection_matrix = Matrix4::I;

    Quaternion rotation = Quaternion::I;
    float rotationPitch = 0.0f;
    float rotationPitchOffset = 0.0f;
    float rotationYaw = 0.0f;
    float rotationYawOffset = 0.0f;
    float rotationRoll = 0.0f;
    float rotationRollOffset = 0.0f;

    Vector3 leftBottomNear_view = Vector3{-1.0f, 1.0f, 0.0f};
    Vector3 rightTopFar_view = Vector3{1.0f, -1.0f, 1.0f};
    ProjectionMode projection_mode = ProjectionMode::Orthographic;
    RenderTargetStack::Node _render_target{};
};
