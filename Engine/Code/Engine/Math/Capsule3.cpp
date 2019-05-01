#include "Engine/Math/Capsule3.hpp"

const Capsule3 Capsule3::UNIT_HORIZONTAL(0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
const Capsule3 Capsule3::UNIT_VERTICAL(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
const Capsule3 Capsule3::UNIT_DEPTH(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f);
const Capsule3 Capsule3::UNIT_CENTERED_HORIZONTAL(-0.5f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f);
const Capsule3 Capsule3::UNIT_CENTERED_VERTICAL(0.0f, -0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f);
const Capsule3 Capsule3::UNIT_CENTERED_DEPTH(0.0f, 0.0f, -0.5f, 0.0f, 0.0f, 0.5f, 1.0f);

Capsule3::Capsule3(const LineSegment3& line, float radius)
    : line(line)
    , radius(radius)
{
    /* DO NOTHING */
}

Capsule3::Capsule3(float startX, float startY, float startZ, float endX, float endY, float endZ, float radius)
    : line(startX, startY, startZ, endX, endY, endZ)
    , radius(radius)
{
    /* DO NOTHING */
}

Capsule3::Capsule3(const Vector3& start_position, const Vector3& end_position, float radius)
    : line(start_position, end_position)
    , radius(radius)
{
    /* DO NOTHING */
}

Capsule3::Capsule3(const Vector3& start_position, const Vector3& direction, float length, float radius)
    : line(start_position, direction.GetNormalize(), length)
    , radius(radius)
{
    /* DO NOTHING */
}

void Capsule3::SetLengthFromStart(float length) {
    line.SetLengthFromStart(length);
}

void Capsule3::SetLengthFromCenter(float length) {
    line.SetLengthFromCenter(length);
}

void Capsule3::SetLengthFromEnd(float length) {
    line.SetLengthFromEnd(length);
}

Vector3 Capsule3::CalcCenter() const {
    return line.CalcCenter();
}

float Capsule3::CalcLength() const {
    return line.CalcLength();
}

float Capsule3::CalcLengthSquared() const {
    return line.CalcLengthSquared();
}

void Capsule3::SetStartEndPositions(const Vector3& start_position, const Vector3& end_position) {
    line.SetStartEndPositions(start_position, end_position);
}

void Capsule3::Translate(const Vector3& translation) {
    line.Translate(translation);
}

Vector3 Capsule3::CalcDisplacement() const {
    return line.CalcDisplacement();
}

Vector3 Capsule3::CalcDirection() const {
    return line.CalcDirection();
}

Capsule3 Capsule3::operator+(const Vector3& translation) const {
    return Capsule3(line + translation, radius);
}

Capsule3 Capsule3::operator-(const Vector3& antiTranslation) const {
    return Capsule3(line - antiTranslation, radius);
}

Capsule3& Capsule3::operator-=(const Vector3& antiTranslation) {
    line -= antiTranslation;
    return *this;
}

Capsule3& Capsule3::operator+=(const Vector3& translation) {
    line += translation;
    return *this;
}
