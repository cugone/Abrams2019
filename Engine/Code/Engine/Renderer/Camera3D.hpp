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
    ~Camera3D() = default;
    explicit Camera3D(const Camera2D& camera2D);
    Camera3D& operator=(const Camera2D& camera2D);

    void SetupView(float fovVerticalDegrees, float aspectRatio = MathUtils::M_16_BY_9_RATIO, float nearDistance = 0.01f, float farDistance = 1.0f, const Vector3& worldUp = Vector3::Y_AXIS);
    void Update(TimeUtils::FPSeconds deltaSeconds);

    const Vector3& GetPosition() const;
    void SetPosition(const Vector3& newPosition);
    void SetPosition(float x, float y, float z);
    void SetPosition(const Vector2& newPosition);
    void SetPosition(float x, float y);
    void Translate(const Vector3& displacement);
    void Translate(float x, float y, float z);
    void Translate(const Vector2& displacement);
    void Translate(float x, float y);

    float CalcFovYDegrees() const;
    float CalcFovXDegrees() const;
    float CalcNearViewWidth() const;
    float CalcNearViewHeight() const;
    float CalcFarViewWidth() const;
    float CalcFarViewHeight() const;

    float GetAspectRatio() const;
    float GetInverseAspectRatio() const;
    float GetNearDistance() const;
    float GetFarDistance() const;

    const Matrix4& GetRotationMatrix() const;
    Matrix4 CreateBillboardMatrix(const Matrix4& rotationMatrix);
    Matrix4 CreateReverseBillboardMatrix(const Matrix4& rotationMatrix);

    const Matrix4& GetViewMatrix() const;
    const Matrix4& GetProjectionMatrix() const;
    const Matrix4& GetViewProjectionMatrix() const;

    const Matrix4& GetInverseViewMatrix() const;
    const Matrix4& GetInverseProjectionMatrix() const;
    const Matrix4& GetInverseViewProjectionMatrix() const;

    Vector3 GetEulerAngles() const;
    void SetEulerAnglesDegrees(const Vector3& eulerAnglesDegrees);
    void SetEulerAngles(const Vector3& eulerAngles);
    void SetForwardFromTarget(const Vector3& lookAtPosition);

    Vector3 GetRight() const;
    Vector3 GetUp() const;
    Vector3 GetForward() const;

    float GetYawDegrees() const;
    float GetPitchDegrees() const;
    float GetRollDegrees() const;

    float GetYaw() const;
    float GetPitch() const;
    float GetRoll() const;

    float trauma = 0.0f;
    float trauma_recovery_rate = 1.0f;
protected:
private:
    void CalcViewMatrix();
    void CalcRotationMatrix();
    void CalcViewProjectionMatrix();
    void CalcProjectionMatrix();

    float aspect_ratio = MathUtils::M_16_BY_9_RATIO;
    float fov_vertical_degrees = 60.0f;
    float near_view_height = 1600.0f;
    float far_view_height = 1600.0f;
    float near_distance = 0.01f;
    float far_distance = 1.0f;
    Vector3 position = Vector3::ZERO;
    Vector3 world_up = Vector3::Y_AXIS;
    Matrix4 view_matrix = Matrix4::GetIdentity();
    Matrix4 rotation_matrix = Matrix4::GetIdentity();
    Matrix4 projection_matrix = Matrix4::GetIdentity();
    Matrix4 view_projection_matrix = Matrix4::GetIdentity();
    Matrix4 inv_view_matrix = Matrix4::GetIdentity();
    Matrix4 inv_projection_matrix = Matrix4::GetIdentity();
    Matrix4 inv_view_projection_matrix = Matrix4::GetIdentity();

    Quaternion rotation = Quaternion::GetIdentity();
    float rotationPitch = 0.0f;
    float rotationYaw = 0.0f;
    float rotationRoll = 0.0f;

};