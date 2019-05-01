#pragma once

#include "Engine/Core/TimeUtils.hpp"

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Matrix4.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"

class Camera2D {
public:

    void SetupView(const Vector2& leftBottom, const Vector2& rightTop, const Vector2& nearFar = Vector2(0.0f, 1.0f), float aspectRatio = MathUtils::M_16_BY_9_RATIO);

    void Update(TimeUtils::FPSeconds deltaSeconds);

    const Vector2& GetPosition() const;
    void SetPosition(const Vector3& newPosition);
    void SetPosition(const Vector2& newPosition);
    void Translate(const Vector3& displacement);
    void Translate(const Vector2& displacement);
    float GetOrientationDegrees() const;
    void SetOrientationDegrees(float newAngleDegrees);
    void ApplyOrientationDegrees(float addAngleDegrees);
    float GetOrientation() const;
    void SetOrientation(float newAngleRadians);
    void ApplyOrientation(float addAngleRadians);
    float GetAspectRatio() const;
    float GetInverseAspectRatio() const;
    float GetNearDistance() const;
    float GetFarDistance() const;

    const Matrix4& GetViewMatrix() const;
    const Matrix4& GetProjectionMatrix() const;
    const Matrix4& GetViewProjectionMatrix() const;

    const Matrix4& GetInverseViewMatrix() const;
    const Matrix4& GetInverseProjectionMatrix() const;
    const Matrix4& GetInverseViewProjectionMatrix() const;

    float trauma = 0.0f;
    float trauma_recovery_rate = 1.0f;
    Vector2 position = Vector2::ZERO;
    float orientation_degrees = 0.0f;
protected:
private:
    void CalcViewMatrix();
    void CalcProjectionMatrix();
    void CalcViewProjectionMatrix();

    Matrix4 view_matrix = Matrix4::GetIdentity();
    Matrix4 projection_matrix = Matrix4::GetIdentity();
    Matrix4 view_projection_matrix = Matrix4::GetIdentity();
    
    Matrix4 inv_view_matrix = Matrix4::GetIdentity();
    Matrix4 inv_projection_matrix = Matrix4::GetIdentity();
    Matrix4 inv_view_projection_matrix = Matrix4::GetIdentity();

    Vector2 leftBottom_view = Vector2{ -1.0f, 1.0f };
    Vector2 rightTop_view = Vector2{ 1.0f, -1.0f };
    Vector2 nearFar_distance = Vector2{ 0.0f, 1.0f };
    float aspect_ratio = MathUtils::M_16_BY_9_RATIO;
};