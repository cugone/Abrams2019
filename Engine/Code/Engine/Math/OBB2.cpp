#include "Engine/Math/OBB2.hpp"

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Matrix4.hpp"

OBB2::OBB2(const Vector2& center, const Vector2& halfExtents, float orientationDegrees)
    : half_extents(halfExtents)
    , position(center)
    , orientationDegrees(orientationDegrees)
{
    /* DO NOTHING */
}

OBB2::OBB2(const Vector2& center, float halfExtentX, float halfExtentY, float orientationDegrees)
    : half_extents(halfExtentX, halfExtentY)
    , position(center)
    , orientationDegrees(orientationDegrees)
{
    /* DO NOTHING */
}

OBB2::OBB2(const Vector2& initialPosition, float initialOrientationDegrees)
    : position(initialPosition)
    , orientationDegrees(initialOrientationDegrees)
{
    /* DO NOTHING */
}

OBB2::OBB2(float initialX, float initialY, float initialOrientationDegrees)
    : position(initialX, initialY)
    , orientationDegrees(initialOrientationDegrees)
{
    /* DO NOTHING */
}

void OBB2::SetOrientationDegrees(float newOrientationDegrees) {
    orientationDegrees = newOrientationDegrees;
}

void OBB2::SetOrientation(float newOrientationRadians) {
    SetOrientationDegrees(MathUtils::ConvertRadiansToDegrees(newOrientationRadians));
}

void OBB2::RotateDegrees(float rotationDegrees) {
    orientationDegrees += rotationDegrees;
}

void OBB2::Rotate(float rotationRadians) {
    RotateDegrees(MathUtils::ConvertRadiansToDegrees(rotationRadians));
}

void OBB2::StretchToIncludePoint(const Vector2& point) {
    const auto disp_to_point = point - position;
    const auto dir_to_point = disp_to_point.GetNormalize();
    Translate(-point);
    AddPaddingToSides(disp_to_point);
}

void OBB2::AddPaddingToSides(float paddingX, float paddingY) {
    AddPaddingToSides(Vector2{paddingX, paddingY});
}

void OBB2::AddPaddingToSides(const Vector2& padding) {
    half_extents += padding;
}

void OBB2::AddPaddingToSidesClamped(float paddingX, float paddingY) {
    const auto half_width = half_extents.x;
    const auto half_height = half_extents.y;

    paddingX = (std::max)(-half_width, paddingX);
    paddingY = (std::max)(-half_height, paddingY);

    AddPaddingToSides(paddingX, paddingY);
}

void OBB2::AddPaddingToSidesClamped(const Vector2& padding) {
    const auto half_width = half_extents.x;
    const auto half_height = half_extents.y;

    const auto clamped_padding = Vector2((std::max)(-half_width, padding.x), (std::max)(-half_height, padding.y));

    AddPaddingToSides(clamped_padding);
}

void OBB2::Translate(const Vector2& translation) {
    position += translation;
}

Vector2 OBB2::GetRight() const {
    auto R = Matrix4::Create2DRotationDegreesMatrix(orientationDegrees);
    return R.TransformDirection(Vector2::X_AXIS);
}

Vector2 OBB2::GetUp() const {
    auto up = GetRight();
    up.Rotate90Degrees();
    return up;
}

Vector2 OBB2::GetLeft() const {
    return -GetRight();
}

Vector2 OBB2::GetDown() const {
    return -GetUp();
}

Vector2 OBB2::CalcDimensions() const {
    return half_extents * 2.0f;
}

Vector2 OBB2::CalcCenter() const {
    return position;
}

OBB2 OBB2::operator+(const Vector2& translation) const {
    return OBB2(position + translation, orientationDegrees);
}

OBB2 OBB2::operator-(const Vector2& antiTranslation) const {
    return OBB2(position - antiTranslation, orientationDegrees);
}

OBB2& OBB2::operator-=(const Vector2& antiTranslation) {
    position -= antiTranslation;
    return *this;
}

OBB2& OBB2::operator+=(const Vector2& translation) {
    position += translation;
    return *this;
}
