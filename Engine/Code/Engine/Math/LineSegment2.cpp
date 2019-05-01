#include "Engine/Math/LineSegment2.hpp"

#include "Engine/Math/MathUtils.hpp"

const LineSegment2 LineSegment2::UNIT_HORIZONTAL(0.0f, 0.0f, 1.0f, 0.0f);
const LineSegment2 LineSegment2::UNIT_VERTICAL(0.0f, 0.0f, 0.0f, 1.0f);
const LineSegment2 LineSegment2::UNIT_CENTERED_HORIZONTAL(-0.5f, 0.0f, 0.5f, 0.0f);
const LineSegment2 LineSegment2::UNIT_CENTERED_VERTICAL(0.0f, -0.5f, 0.0f, 0.5f);

LineSegment2::LineSegment2(float startX, float startY, float endX, float endY)
    : start(startX, startY)
    , end(endX, endY)
{
    /* DO NOTHING */
}

LineSegment2::LineSegment2(const Vector2& startPosition, const Vector2& endPosition)
    : start(startPosition)
    , end(endPosition)
{
    /* DO NOTHING */
}

LineSegment2::LineSegment2(const Vector2& startPosition, const Vector2& direction, float length)
    :start(startPosition)
    , end(startPosition + direction.GetNormalize() * length)
{
    /* DO NOTHING */
}

LineSegment2::LineSegment2(const Vector2& startPosition, float angleDegrees, float length)
{
    end.SetXY(length * MathUtils::CosDegrees(angleDegrees), length * MathUtils::SinDegrees(angleDegrees));
    start += startPosition;
    end += startPosition;
}

void LineSegment2::SetLengthFromStart(float length) {
    float angleDegrees = CalcDisplacement().CalcHeadingDegrees();
    end = start + Vector2(length * MathUtils::CosDegrees(angleDegrees),
                          length * MathUtils::SinDegrees(angleDegrees));
}

void LineSegment2::SetLengthFromCenter(float length) {
    float angleDegrees = CalcDisplacement().CalcHeadingDegrees();
    float half_length = length * 0.5f;

    Vector2 center = CalcCenter();
    Vector2 half_extent(half_length * MathUtils::CosDegrees(angleDegrees),
                        half_length * MathUtils::SinDegrees(angleDegrees));

    start = center - half_extent;
    end = center + half_extent;
}

void LineSegment2::SetLengthFromEnd(float length) {
    float angleDegrees = CalcDisplacement().CalcHeadingDegrees();
    start = end - Vector2(length * MathUtils::CosDegrees(angleDegrees),
                          length * MathUtils::SinDegrees(angleDegrees));
}

Vector2 LineSegment2::CalcCenter() const {
    Vector2 displacement = CalcDisplacement();
    return start + displacement * 0.5f;
}

float LineSegment2::CalcLength() const {
    return (end - start).CalcLength();
}

float LineSegment2::CalcLengthSquared() const {
    return (end - start).CalcLengthSquared();
}

void LineSegment2::SetDirectionFromStart(float angleDegrees) {
    Vector2 t = start;
    Translate(-t);
    SetAngle(angleDegrees);
    Translate(t);
}

void LineSegment2::SetDirectionFromCenter(float angleDegrees) {
    Vector2 center = CalcCenter();
    Translate(-center);
    SetAngle(angleDegrees);
    Translate(center);
}

void LineSegment2::SetDirectionFromEnd(float angleDegrees) {
    Vector2 t = end;
    Translate(-t);
    SetAngle(angleDegrees);
    Translate(t);
}

void LineSegment2::SetStartEndPositions(const Vector2& startPosition, const Vector2& endPosition) {
    start = startPosition;
    end = endPosition;
}

void LineSegment2::Translate(const Vector2& translation) {
    start += translation;
    end += translation;
}

void LineSegment2::Rotate(float angleDegrees) {
    float heading = (end - start).CalcHeadingDegrees();
    SetAngle(heading + angleDegrees);
}

void LineSegment2::RotateStartPosition(float angleDegrees) {
    Vector2 t = end;
    Translate(-t);
    Rotate(angleDegrees);
    Translate(t);
}

void LineSegment2::RotateEndPosition(float angleDegrees) {
    Vector2 t = start;
    Translate(-t);
    Rotate(angleDegrees);
    Translate(t);
}

void LineSegment2::Rotate90Degrees() {
    Rotate(90.0f);
}

void LineSegment2::RotateNegative90Degrees() {
    Rotate(-90.0f);
}

void LineSegment2::Rotate180Degrees() {
    std::swap(start, end);
}

Vector2 LineSegment2::CalcDisplacement() const {
    return end - start;
}

Vector2 LineSegment2::CalcDirection() const {
    return (end - start).GetNormalize();
}

Vector2 LineSegment2::CalcPositiveNormal() const {
    Vector2 dir = CalcDirection();
    dir.Rotate90Degrees();
    return dir;
}

Vector2 LineSegment2::CalcNegativeNormal() const {
    Vector2 dir = CalcDirection();
    dir.RotateNegative90Degrees();
    return dir;
}

LineSegment2 LineSegment2::operator+(const Vector2& translation) const {
    return LineSegment2(start + translation, end + translation);
}

LineSegment2 LineSegment2::operator-(const Vector2& antiTranslation) const {
    return LineSegment2(start - antiTranslation, end - antiTranslation);
}

LineSegment2& LineSegment2::operator-=(const Vector2& antiTranslation) {
    start -= antiTranslation;
    end -= antiTranslation;
    return *this;
}

LineSegment2& LineSegment2::operator+=(const Vector2& translation) {
    start += translation;
    end += translation;
    return *this;
}

void LineSegment2::SetAngle(float angleDegrees) {
    float oldX = start.x;
    float oldY = start.y;

    float c = MathUtils::CosDegrees(angleDegrees);
    float s = MathUtils::SinDegrees(angleDegrees);

    start.x = oldX * c - oldY * s;
    start.y = oldX * s + oldY * c;
    
    oldX = end.y;
    oldY = end.y;

    end.x = oldX * c - oldY * s;
    end.y = oldX * c + oldY * s;

}
