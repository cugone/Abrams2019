#include "Engine/Renderer/Camera2D.hpp"

void Camera2D::SetupView(const Vector2& leftBottom, const Vector2& rightTop, const Vector2& nearFar /*= Vector2(0.0f, 1.0f)*/, float aspectRatio /*= MathUtils::M_16_BY_9_RATIO*/) {
    leftBottom_view = leftBottom;
    rightTop_view = rightTop;
    aspect_ratio = aspectRatio;
    nearFar_distance = nearFar;
    CalcViewMatrix();
    CalcProjectionMatrix();
    CalcViewProjectionMatrix();
}

void Camera2D::CalcViewProjectionMatrix() {
    view_projection_matrix = projection_matrix * view_matrix;
    inv_view_projection_matrix = Matrix4::CalculateInverse(view_projection_matrix);
}

void Camera2D::CalcProjectionMatrix() {
    projection_matrix = Matrix4::CreateDXOrthographicProjection(leftBottom_view.x, rightTop_view.x, leftBottom_view.y, rightTop_view.y, nearFar_distance.x, nearFar_distance.y);
    inv_projection_matrix = Matrix4::CalculateInverse(projection_matrix);
}

void Camera2D::CalcViewMatrix() {
    Matrix4 vT = Matrix4::CreateTranslationMatrix(-position);
    Matrix4 vR = Matrix4::Create2DRotationDegreesMatrix(orientation_degrees);
    view_matrix = vT * vR;
    inv_view_matrix = Matrix4::CalculateInverse(view_matrix);
}

void Camera2D::Update(TimeUtils::FPSeconds deltaSeconds) {
    trauma -= trauma_recovery_rate * deltaSeconds.count();
}

const Vector2& Camera2D::GetPosition() const {
    return position;
}

void Camera2D::SetPosition(const Vector3& newPosition) {
    SetPosition(Vector2(newPosition));
}

void Camera2D::SetPosition(const Vector2& newPosition) {
    position = newPosition;
}

void Camera2D::Translate(const Vector3& displacement) {
    Translate(Vector2{ displacement.x, displacement.y });
}

void Camera2D::Translate(const Vector2& displacement) {
    position += displacement;
}

float Camera2D::GetOrientationDegrees() const {
    return orientation_degrees;
}

void Camera2D::SetOrientationDegrees(float newAngleDegrees) {
    orientation_degrees = newAngleDegrees;
}

void Camera2D::ApplyOrientationDegrees(float addAngleDegrees) {
    orientation_degrees += addAngleDegrees;
}

float Camera2D::GetOrientation() const {
    return MathUtils::ConvertDegreesToRadians(orientation_degrees);
}

void Camera2D::SetOrientation(float newAngleRadians) {
    orientation_degrees = MathUtils::ConvertRadiansToDegrees(newAngleRadians);
}

void Camera2D::ApplyOrientation(float addAngleRadians) {
    orientation_degrees += MathUtils::ConvertRadiansToDegrees(addAngleRadians);
}

float Camera2D::GetAspectRatio() const {
    return aspect_ratio;
}

float Camera2D::GetInverseAspectRatio() const {
    return 1.0f / aspect_ratio;
}

float Camera2D::GetNearDistance() const {
    return nearFar_distance.x;
}

float Camera2D::GetFarDistance() const {
    return nearFar_distance.y;
}

const Matrix4& Camera2D::GetViewMatrix() const {
    return view_matrix;
}

const Matrix4& Camera2D::GetProjectionMatrix() const {
    return projection_matrix;
}

const Matrix4& Camera2D::GetViewProjectionMatrix() const {
    return view_projection_matrix;
}

const Matrix4& Camera2D::GetInverseViewMatrix() const {
    return inv_view_matrix;
}

const Matrix4& Camera2D::GetInverseProjectionMatrix() const {
    return inv_projection_matrix;
}

const Matrix4& Camera2D::GetInverseViewProjectionMatrix() const {
    return inv_view_projection_matrix;
}
