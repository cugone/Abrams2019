#pragma once

#include "Engine/Core/TimeUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Matrix4.hpp"
#include "Engine/Math/Quaternion.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"

class Camera2D;

class Camera3D {
public:
    Camera3D() = default;
    Camera3D(const Camera3D& camera3D) noexcept = default;
    Camera3D(Camera3D&& camera3D) noexcept = default;
    Camera3D& operator=(const Camera3D& camera3D) noexcept = default;
    Camera3D& operator=(Camera3D&& camera3D) noexcept = default;
    ~Camera3D() = default;

    explicit Camera3D(const Camera2D& camera2D) noexcept;
    Camera3D& operator=(const Camera2D& camera2D) noexcept;

    void SetupView(float fovVerticalDegrees, float aspectRatio = MathUtils::M_16_BY_9_RATIO, float nearDistance = 0.01f, float farDistance = 1.0f, const Vector3& worldUp = Vector3::Y_AXIS) noexcept;
    void Update(TimeUtils::FPSeconds deltaSeconds) noexcept;

    const Vector3& GetPosition() const noexcept;
    void SetPosition(const Vector3& newPosition) noexcept;
    void SetPosition(float x, float y, float z) noexcept;
    void SetPosition(const Vector2& newPosition) noexcept;
    void SetPosition(float x, float y) noexcept;
    void Translate(const Vector3& displacement) noexcept;
    void Translate(float x, float y, float z) noexcept;
    void Translate(const Vector2& displacement) noexcept;
    void Translate(float x, float y) noexcept;

    float CalcFovYDegrees() const noexcept;
    float CalcFovXDegrees() const noexcept;
    float CalcNearViewWidth() const noexcept;
    float CalcNearViewHeight() const noexcept;
    float CalcFarViewWidth() const noexcept;
    float CalcFarViewHeight() const noexcept;

    float GetAspectRatio() const noexcept;
    float GetInverseAspectRatio() const noexcept;
    float GetNearDistance() const noexcept;
    float GetFarDistance() const noexcept;

    const Matrix4& GetRotationMatrix() const noexcept;
    Matrix4 CreateBillboardMatrix(const Matrix4& rotationMatrix) noexcept;
    Matrix4 CreateReverseBillboardMatrix(const Matrix4& rotationMatrix) noexcept;

    const Matrix4& GetViewMatrix() const noexcept;
    const Matrix4& GetProjectionMatrix() const noexcept;
    const Matrix4& GetViewProjectionMatrix() const noexcept;

    const Matrix4& GetInverseViewMatrix() const noexcept;
    const Matrix4& GetInverseProjectionMatrix() const noexcept;
    const Matrix4& GetInverseViewProjectionMatrix() const noexcept;

    Vector3 GetEulerAngles() const noexcept;
    void SetEulerAnglesDegrees(const Vector3& eulerAnglesDegrees) noexcept;
    void SetEulerAngles(const Vector3& eulerAngles) noexcept;
    void SetForwardFromTarget(const Vector3& lookAtPosition) noexcept;

    void Rotate(const Vector3& axis, const float angle) noexcept;
    void RotateDegrees(const Vector3& axis, const float angleDegrees) noexcept;

    void RotateBy(const Vector3& eulerAngles) noexcept;
    void RotateDegreesBy(const Vector3& eulerAnglesDegrees) noexcept;

    void RotatePitchDegreesBy(float angleDegrees) noexcept;
    void RotateYawDegreesBy(float angleDegrees) noexcept;
    void RotateRollDegreesBy(float angleDegrees) noexcept;
    
    void RotatePitchBy(float angleDegrees) noexcept;
    void RotateYawBy(float angleDegrees) noexcept;
    void RotateRollBy(float angleDegrees) noexcept;

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
    void CalcViewMatrix() noexcept;
    void CalcRotationMatrix() noexcept;
    void CalcViewProjectionMatrix() noexcept;
    void CalcProjectionMatrix() noexcept;

    float aspect_ratio = MathUtils::M_16_BY_9_RATIO;
    float fov_vertical_degrees = 60.0f;
    float near_view_height = 1600.0f;
    float far_view_height = 1600.0f;
    float near_distance = 0.01f;
    float far_distance = 1.0f;
    Vector3 position = Vector3::ZERO;
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
    float rotationYaw = 0.0f;
    float rotationRoll = 0.0f;
};