#include "Engine/Math/Capsule2.hpp"

const Capsule2 Capsule2::UNIT_HORIZONTAL(0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
const Capsule2 Capsule2::UNIT_VERTICAL(0.0f, 0.0f, 0.0f, 1.0f, 1.0f);
const Capsule2 Capsule2::UNIT_CENTERED_HORIZONTAL(-0.5f, 0.0f, 0.5f, 0.0f, 1.0f);
const Capsule2 Capsule2::UNIT_CENTERED_VERTICAL(0.0f, -0.5f, 0.0f, 0.5f, 1.0f);

Capsule2::Capsule2(const LineSegment2& line, float radius)
    : line(line)
    , radius(radius)
{
    /* DO NOTHING */
}
Capsule2::Capsule2(float startX, float startY, float endX, float endY, float radius)
    : line(startX, startY, endX, endY)
    , radius(radius)
{
    /* DO NOTHING */
}
Capsule2::Capsule2(const Vector2& start_position, const Vector2& end_position, float radius)
    : line(start_position, end_position)
    , radius(radius)
{
    /* DO NOTHING */
}

Capsule2::Capsule2(const Vector2& start_position, const Vector2& direction, float length, float radius)
    : line(start_position, direction, length)
    , radius(radius)
{
    /* DO NOTHING */
}

Capsule2::Capsule2(const Vector2& start_position, float angle_degrees, float length, float radius)
    : line(start_position, angle_degrees, length)
    , radius(radius)
{
    /* DO NOTHING */
}

void Capsule2::SetLengthFromStart(float length) {
    line.SetLengthFromStart(length);
}

void Capsule2::SetLengthFromCenter(float length) {
    line.SetLengthFromCenter(length);
}

void Capsule2::SetLengthFromEnd(float length) {
    line.SetLengthFromEnd(length);
}

Vector2 Capsule2::CalcCenter() const {
    return line.CalcCenter();
}

float Capsule2::CalcLength() const {
    return line.CalcLength();
}

float Capsule2::CalcLengthSquared() const {
    return line.CalcLengthSquared();
}

void Capsule2::SetDirectionFromStart(float angle_degrees) {
    line.SetDirectionFromStart(angle_degrees);
}

void Capsule2::SetDirectionFromCenter(float angle_degrees) {
    line.SetDirectionFromCenter(angle_degrees);
}

void Capsule2::SetDirectionFromEnd(float angle_degrees) {
    line.SetDirectionFromEnd(angle_degrees);
}

void Capsule2::SetStartEndPositions(const Vector2& start_position, const Vector2& end_position) {
    line.SetStartEndPositions(start_position, end_position);
}

void Capsule2::Translate(const Vector2& translation) {
    line.Translate(translation);
}

void Capsule2::Rotate(float angle_degrees) {
    line.Rotate(angle_degrees);
}

void Capsule2::RotateStartPosition(float angle_degrees) {
    line.RotateStartPosition(angle_degrees);
}

void Capsule2::RotateEndPosition(float angle_degrees) {
    line.RotateEndPosition(angle_degrees);
}

void Capsule2::Rotate90Degrees() {
    line.Rotate90Degrees();
}

void Capsule2::RotateNegative90Degrees() {
    line.RotateNegative90Degrees();
}

void Capsule2::Rotate180Degrees() {
    line.Rotate180Degrees();
}

Vector2 Capsule2::CalcDisplacement() const {
    return line.CalcDisplacement();
}

Vector2 Capsule2::CalcDirection() const {
    return line.CalcDirection();
}

Vector2 Capsule2::CalcPositiveNormal() const {
    return line.CalcPositiveNormal();
}

Vector2 Capsule2::CalcNegativeNormal() const {
    return line.CalcNegativeNormal();
}

Capsule2 Capsule2::operator+(const Vector2& translation) const {
    return Capsule2(line + translation, radius);
}

Capsule2 Capsule2::operator-(const Vector2& antiTranslation) const {
    return Capsule2(line - antiTranslation, radius);
}

Capsule2& Capsule2::operator-=(const Vector2& antiTranslation) {
    line -= antiTranslation;
    return *this;
}

Capsule2& Capsule2::operator+=(const Vector2& translation) {
    line += translation;
    return *this;
}

void Capsule2::SetAngle(float angle_degrees) {
    line.SetAngle(angle_degrees);
}
