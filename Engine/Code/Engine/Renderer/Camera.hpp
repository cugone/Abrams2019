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
    const ProjectionMode& GetProjectionMode() const noexcept;

    void Update(TimeUtils::FPSeconds deltaSeconds) noexcept;

    const Vector3& GetPosition() const noexcept;
    void SetPosition(const Vector3& newPosition) noexcept;
    void SetPosition(const Vector2& newPosition) noexcept;

    void Translate(const Vector3& displacement) noexcept;
    void Translate(const Vector2& displacement) noexcept;

    void SetOffsets(const Matrix4& transform, float fov) noexcept;
    void AddOffsets(const Matrix4& transform, float fov) noexcept;
    void ClearOffsets() noexcept;

    float GetAspectRatio() const noexcept;
    float GetInverseAspectRatio() const noexcept;
    float GetNearDistance() const noexcept;
    float GetFarDistance() const noexcept;

    const Matrix4& GetViewMatrix() const noexcept;
    const Matrix4& GetProjectionMatrix() const noexcept;
    const Matrix4& GetViewProjectionMatrix() const noexcept;

    const Matrix4& GetInverseViewMatrix() const noexcept;
    const Matrix4& GetInverseProjectionMatrix() const noexcept;
    const Matrix4& GetInverseViewProjectionMatrix() const noexcept;

    const RenderTargetStack::Node& GetRenderTarget() const noexcept;
    RenderTargetStack::Node& GetRenderTarget() noexcept;

    float GetShake() const noexcept;

    const Matrix4& GetRotationMatrix() const noexcept;
    Matrix4 CreateBillboardMatrix(const Matrix4& rotationMatrix) noexcept;
    Matrix4 CreateReverseBillboardMatrix(const Matrix4& rotationMatrix) noexcept;

    Vector3 GetEulerAngles() const noexcept;
    void SetEulerAnglesDegrees(const Vector3& eulerAnglesDegrees) noexcept;
    void SetEulerAngles(const Vector3& eulerAngles) noexcept;
    void SetForwardFromTarget(const Vector3& lookAtPosition) noexcept;

    Vector3 GetRight() const noexcept;
    Vector3 GetUp() const noexcept;
    Vector3 GetForward() const noexcept;

    float GetYawDegrees() const noexcept;
    float GetPitchDegrees() const noexcept;
    float GetRollDegrees() const noexcept;

    float GetYaw() const noexcept;
    float GetPitch() const noexcept;
    float GetRoll() const noexcept;

    float trauma = 0.0f;
    float trauma_recovery_rate = 1.0f;

protected:
private:

    float aspect_ratio = MathUtils::M_16_BY_9_RATIO;
    float fovH = 60.0f;
    float orthoWidth = 1600.0f;
    float near_distance = 0.01f;
    float far_distance = 1.0f;
    Vector3 position = Vector3::ZERO;
    Vector3 offset = Vector3::ZERO;
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
    float pitchOffset = 0.0f;
    float rotationYaw = 0.0f;
    float yawOffset = 0.0f;
    float rotationRoll = 0.0f;
    float rollOffset = 0.0f;

    Vector3 leftBottomNear_view = Vector3{-1.0f, 1.0f, 0.01f};
    Vector3 rightTopFar_view = Vector3{1.0f, -1.0f, 1.0f};
    ProjectionMode perspective_mode = ProjectionMode::Orthographic;
    RenderTargetStack::Node _render_target{};
};
