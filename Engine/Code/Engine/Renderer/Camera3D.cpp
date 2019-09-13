#include "Engine/Renderer/Camera3D.hpp"

#include "Engine/Renderer/Camera2D.hpp"

#include <algorithm>

Camera3D::Camera3D(const Camera2D& camera2D) noexcept
    : trauma(camera2D.trauma)
    , trauma_recovery_rate(camera2D.trauma_recovery_rate)
    , aspect_ratio(camera2D.GetAspectRatio())
    , far_distance(camera2D.GetFarDistance())
    , near_distance(camera2D.GetNearDistance())
    , position(camera2D.GetPosition())
    , rotationRoll(camera2D.GetOrientation())
    , view_matrix(camera2D.GetViewMatrix())
    , projection_matrix(camera2D.GetProjectionMatrix())
    , view_projection_matrix(camera2D.GetViewProjectionMatrix())
    , inv_view_matrix(camera2D.GetInverseViewMatrix())
    , inv_projection_matrix(camera2D.GetInverseProjectionMatrix())
    , inv_view_projection_matrix(camera2D.GetInverseViewProjectionMatrix())
    , rotation_matrix(camera2D.GetViewMatrix().GetRotation())
    , rotation(rotation_matrix)
{
    /* DO NOTHING */
}

Camera3D& Camera3D::operator=(const Camera2D& camera2D) noexcept {
    trauma = camera2D.trauma;
    trauma_recovery_rate = camera2D.trauma_recovery_rate;
    aspect_ratio = camera2D.GetAspectRatio();
    far_distance = camera2D.GetFarDistance();
    near_distance = (std::max)(0.01f, camera2D.GetNearDistance());
    position = Vector3{camera2D.GetPosition()};
    rotationRoll = camera2D.GetOrientation();
    view_matrix = camera2D.GetViewMatrix();
    projection_matrix = camera2D.GetProjectionMatrix();
    view_projection_matrix = camera2D.GetViewProjectionMatrix();
    inv_view_matrix = camera2D.GetInverseViewMatrix();
    inv_projection_matrix = camera2D.GetInverseProjectionMatrix();
    inv_view_projection_matrix = camera2D.GetInverseViewProjectionMatrix();
    rotation_matrix = camera2D.GetViewMatrix().GetRotation();
    rotation = Quaternion{ rotation_matrix };
    return *this;
}

void Camera3D::SetupView(float fovVerticalDegrees, float aspectRatio /*= MathUtils::M_16_BY_9_RATIO*/, float nearDistance /*= 0.01f*/, float farDistance /*= 1.0f*/, const Vector3& worldUp /*= Vector3::Y_AXIS*/) noexcept {
    fov_vertical_degrees = fovVerticalDegrees;
    aspect_ratio = aspectRatio;
    near_distance = (std::max)(0.01f, nearDistance);
    far_distance = farDistance;
    world_up = worldUp.GetNormalize();
    near_view_height = 2.0f * near_distance * std::tan(0.5f * fov_vertical_degrees);
    far_view_height = 2.0f * far_distance * std::tan(0.5f * fov_vertical_degrees);
    CalcRotationMatrix();
    CalcViewMatrix();
    CalcProjectionMatrix();
    CalcViewProjectionMatrix();
}

void Camera3D::CalcViewProjectionMatrix() noexcept {
    view_projection_matrix = projection_matrix * view_matrix;
    inv_view_projection_matrix = Matrix4::CalculateInverse(view_projection_matrix);
}

void Camera3D::CalcProjectionMatrix() noexcept {
    projection_matrix = Matrix4::CreateDXPerspectiveProjection(fov_vertical_degrees, aspect_ratio, near_distance, far_distance);
    inv_projection_matrix = Matrix4::CalculateInverse(projection_matrix);
}

Matrix4 Camera3D::CreateBillboardMatrix(const Matrix4& rotationMatrix) noexcept {
    return inv_view_matrix.GetRotation() * Matrix4::Create3DYRotationDegreesMatrix(180.0f) * rotationMatrix;
}

Matrix4 Camera3D::CreateReverseBillboardMatrix(const Matrix4& rotationMatrix) noexcept {
    return inv_view_matrix.GetRotation() * rotationMatrix;
}

Vector3 Camera3D::GetEulerAngles() const noexcept {
    return Vector3{rotationPitch, rotationYaw, rotationRoll};
}

void Camera3D::CalcViewMatrix() noexcept {

    Matrix4 vT = Matrix4::CreateTranslationMatrix(-position);
    Matrix4 vQ = rotation_matrix;
    view_matrix = vQ * vT;
    inv_view_matrix = Matrix4::CalculateInverse(view_matrix);
}

void Camera3D::CalcRotationMatrix() noexcept {
    float c_x_theta = MathUtils::CosDegrees(rotationPitch);
    float s_x_theta = MathUtils::SinDegrees(rotationPitch);
    Matrix4 Rx;
    Rx.SetIBasis(Vector4(1.0f, 0.0f, 0.0f, 0.0f));
    Rx.SetJBasis(Vector4(0.0f, c_x_theta, s_x_theta, 0.0f));
    Rx.SetKBasis(Vector4(0.0f, -s_x_theta, c_x_theta, 0.0f));

    float c_y_theta = MathUtils::CosDegrees(rotationYaw);
    float s_y_theta = MathUtils::SinDegrees(rotationYaw);
    Matrix4 Ry;
    Ry.SetIBasis(Vector4(c_y_theta, 0.0f, -s_y_theta, 0.0f));
    Ry.SetJBasis(Vector4(0.0f, 1.0f, 0.0f, 0.0f));
    Ry.SetKBasis(Vector4(s_y_theta, 0.0f, c_y_theta, 0.0f));

    float c_z_theta = MathUtils::CosDegrees(rotationRoll);
    float s_z_theta = MathUtils::SinDegrees(rotationRoll);
    Matrix4 Rz;
    Rz.SetIBasis(Vector4(c_z_theta, s_z_theta, 0.0f, 0.0f));
    Rz.SetJBasis(Vector4(-s_z_theta, c_z_theta, 0.0f, 0.0f));
    Rz.SetKBasis(Vector4(0.0f, 0.0f, 1.0f, 0.0f));

    Matrix4 R = Rz * Rx * Ry;
    rotation_matrix = R;
}

void Camera3D::Update(TimeUtils::FPSeconds deltaSeconds) noexcept {
    trauma -= trauma_recovery_rate * deltaSeconds.count();
}

const Vector3& Camera3D::GetPosition() const noexcept {
    return position;
}

void Camera3D::SetPosition(const Vector3& newPosition) noexcept {
    position = newPosition;
}

void Camera3D::SetPosition(float x, float y, float z) noexcept {
    SetPosition(Vector3{ x, y, z });
}

void Camera3D::SetPosition(const Vector2& newPosition) noexcept {
    SetPosition(newPosition.x, newPosition.y, 0.0f);
}

void Camera3D::SetPosition(float x, float y) noexcept {
    SetPosition(Vector2{ x, y });
}

void Camera3D::Translate(const Vector3& displacement) noexcept {
    position += displacement;
}

void Camera3D::Translate(float x, float y, float z) noexcept {
    Translate(Vector3{ x, y, z });
}

void Camera3D::Translate(const Vector2& displacement) noexcept {
    Translate(displacement.x, displacement.y, 0.0f);
}

void Camera3D::Translate(float x, float y) noexcept {
    Translate(Vector2{ x, y });
}

float Camera3D::CalcFovYDegrees() const noexcept {
    return fov_vertical_degrees;
}

float Camera3D::CalcFovXDegrees() const noexcept {
    float half_width = 0.5f * CalcNearViewWidth();
    return 2.0f * std::atan(half_width / near_distance);
}

float Camera3D::CalcNearViewWidth() const noexcept {
    return aspect_ratio * near_view_height;
}

float Camera3D::CalcNearViewHeight() const noexcept {
    return near_view_height;
}

float Camera3D::CalcFarViewWidth() const noexcept {
    return aspect_ratio * far_view_height;
}

float Camera3D::CalcFarViewHeight() const noexcept {
    return far_view_height;
}

float Camera3D::GetAspectRatio() const noexcept {
    return aspect_ratio;
}

float Camera3D::GetInverseAspectRatio() const noexcept {
    return 1.0f / aspect_ratio;
}

float Camera3D::GetNearDistance() const noexcept {
    return near_distance;
}

float Camera3D::GetFarDistance() const noexcept {
    return far_distance;
}

const Matrix4& Camera3D::GetRotationMatrix() const noexcept {
    return rotation_matrix;
}

const Matrix4& Camera3D::GetViewMatrix() const noexcept {
    return view_matrix;
}

const Matrix4& Camera3D::GetProjectionMatrix() const noexcept {
    return projection_matrix;
}

const Matrix4& Camera3D::GetViewProjectionMatrix() const noexcept {
    return view_projection_matrix;
}

const Matrix4& Camera3D::GetInverseViewMatrix() const noexcept {
    return inv_view_matrix;
}

const Matrix4& Camera3D::GetInverseProjectionMatrix() const noexcept {
    return inv_projection_matrix;
}

const Matrix4& Camera3D::GetInverseViewProjectionMatrix() const noexcept {
    return inv_view_projection_matrix;
}

void Camera3D::SetEulerAngles(const Vector3& eulerAngles) noexcept {
    rotationPitch = eulerAngles.x;
    rotationYaw = eulerAngles.y;
    rotationRoll = eulerAngles.z;
}

void Camera3D::SetEulerAnglesDegrees(const Vector3& eulerAnglesDegrees) noexcept {
    SetEulerAngles(Vector3{ MathUtils::ConvertDegreesToRadians(eulerAnglesDegrees.x)
                   , MathUtils::ConvertDegreesToRadians(eulerAnglesDegrees.y)
                   , MathUtils::ConvertDegreesToRadians(eulerAnglesDegrees.z) }
                   );
}

void Camera3D::SetForwardFromTarget(const Vector3& lookAtPosition) noexcept {
    Vector3 forward = (lookAtPosition - position).GetNormalize();
    Vector3 right = MathUtils::CrossProduct(world_up.GetNormalize(), forward);
    Vector3 up = MathUtils::CrossProduct(forward, right);
    Matrix4 m;
    m.SetIBasis(Vector4(right, 0.0f));
    m.SetJBasis(Vector4(up, 0.0f));
    m.SetKBasis(Vector4(forward, 0.0f));
    rotation = Quaternion(m);
    auto eulerangles = rotation.CalcEulerAnglesDegrees();
    rotationPitch = eulerangles.x;
    rotationYaw = eulerangles.y;
    rotationRoll = eulerangles.z;
}

Vector3 Camera3D::GetRight() const noexcept {
    auto forward = GetForward();
    return MathUtils::CrossProduct(world_up, forward);
}

Vector3 Camera3D::GetUp() const noexcept {
    auto forward = GetForward();
    auto right = GetRight();
    return MathUtils::CrossProduct(forward, right);
}

Vector3 Camera3D::GetForward() const noexcept {
    float cos_yaw = MathUtils::CosDegrees(rotationYaw);
    float cos_pitch = MathUtils::CosDegrees(rotationPitch);

    float sin_yaw = MathUtils::SinDegrees(rotationYaw);
    float sin_pitch = MathUtils::SinDegrees(rotationPitch);

    return Vector3(-sin_yaw * cos_pitch, sin_pitch, cos_yaw * cos_pitch);
}

float Camera3D::GetYawDegrees() const noexcept {
    return MathUtils::ConvertRadiansToDegrees(GetYaw());
}

float Camera3D::GetPitchDegrees() const noexcept {
    return MathUtils::ConvertRadiansToDegrees(GetPitch());
}

float Camera3D::GetRollDegrees() const noexcept {
    return MathUtils::ConvertRadiansToDegrees(GetRoll());
}

float Camera3D::GetYaw() const noexcept {
    return rotationYaw;
}

float Camera3D::GetPitch() const noexcept {
    return rotationPitch;
}

float Camera3D::GetRoll() const noexcept {
    return rotationRoll;
}
