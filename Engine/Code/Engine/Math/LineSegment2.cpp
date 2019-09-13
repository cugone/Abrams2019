#include "Engine/Math/LineSegment2.hpp"

#include "Engine/Math/MathUtils.hpp"

const LineSegment2 LineSegment2::UNIT_HORIZONTAL(0.0f, 0.0f, 1.0f, 0.0f);
const LineSegment2 LineSegment2::UNIT_VERTICAL(0.0f, 0.0f, 0.0f, 1.0f);
const LineSegment2 LineSegment2::UNIT_CENTERED_HORIZONTAL(-0.5f, 0.0f, 0.5f, 0.0f);
const LineSegment2 LineSegment2::UNIT_CENTERED_VERTICAL(0.0f, -0.5f, 0.0f, 0.5f);

LineSegment2::LineSegment2(float startX, float startY, float endX, float endY) noexcept
    : start(startX, startY)
    , end(endX, endY)
{
    /* DO NOTHING */
}

LineSegment2::LineSegment2(const Vector2& startPosition, const Vector2& endPosition) noexcept
    : start(startPosition)
    , end(endPosition)
{
    /* DO NOTHING */
}

LineSegment2::LineSegment2(const Vector2& startPosition, const Vector2& direction, float length) noexcept
    :start(startPosition)
    , end(startPosition + direction.GetNormalize() * length)
{
    /* DO NOTHING */
}

LineSegment2::LineSegment2(const Vector2& startPosition, float angleDegrees, float length) noexcept
{
    end.SetXY(length * MathUtils::CosDegrees(angleDegrees), length * MathUtils::SinDegrees(angleDegrees));
    start += startPosition;
    end += startPosition;
}

void LineSegment2::SetLengthFromStart(float length) noexcept {
    float angleDegrees = CalcDisplacement().CalcHeadingDegrees();
    end = start + Vector2(length * MathUtils::CosDegrees(angleDegrees),
                          length * MathUtils::SinDegrees(angleDegrees));
}

void LineSegment2::SetLengthFromCenter(float length) noexcept {
    float angleDegrees = CalcDisplacement().CalcHeadingDegrees();
    float half_length = length * 0.5f;

    Vector2 center = CalcCenter();
    Vector2 half_extent(half_length * MathUtils::CosDegrees(angleDegrees),
                        half_length * MathUtils::SinDegrees(angleDegrees));

    start = center - half_extent;
    end = center + half_extent;
}

void LineSegment2::SetLengthFromEnd(float length) noexcept {
    float angleDegrees = CalcDisplacement().CalcHeadingDegrees();
    start = end - Vector2(length * MathUtils::CosDegrees(angleDegrees),
                          length * MathUtils::SinDegrees(angleDegrees));
}

Vector2 LineSegment2::CalcCenter() const noexcept {
    Vector2 displacement = CalcDisplacement();
    return start + displacement * 0.5f;
}

float LineSegment2::CalcLength() const noexcept {
    return (end - start).CalcLength();
}

float LineSegment2::CalcLengthSquared() const noexcept {
    return (end - start).CalcLengthSquared();
}

void LineSegment2::SetDirectionFromStart(float angleDegrees) noexcept {
    Vector2 t = start;
    Translate(-t);
    SetAngle(angleDegrees);
    Translate(t);
}

void LineSegment2::SetDirectionFromCenter(float angleDegrees) noexcept {
    Vector2 center = CalcCenter();
    Translate(-center);
    SetAngle(angleDegrees);
    Translate(center);
}

void LineSegment2::SetDirectionFromEnd(float angleDegrees) noexcept {
    Vector2 t = end;
    Translate(-t);
    SetAngle(angleDegrees);
    Translate(t);
}

void LineSegment2::SetStartEndPositions(const Vector2& startPosition, const Vector2& endPosition) noexcept {
    start = startPosition;
    end = endPosition;
}

void LineSegment2::Translate(const Vector2& translation) noexcept {
    start += translation;
    end += translation;
}

void LineSegment2::Rotate(float angleDegrees) noexcept {
    float heading = (end - start).CalcHeadingDegrees();
    SetAngle(heading + angleDegrees);
}

void LineSegment2::RotateStartPosition(float angleDegrees) noexcept {
    Vector2 t = end;
    Translate(-t);
    Rotate(angleDegrees);
    Translate(t);
}

void LineSegment2::RotateEndPosition(float angleDegrees) noexcept {
    Vector2 t = start;
    Translate(-t);
    Rotate(angleDegrees);
    Translate(t);
}

void LineSegment2::Rotate90Degrees() noexcept {
    Rotate(90.0f);
}

void LineSegment2::RotateNegative90Degrees() noexcept {
    Rotate(-90.0f);
}

void LineSegment2::Rotate180Degrees() noexcept {
    std::swap(start, end);
}

Vector2 LineSegment2::CalcDisplacement() const noexcept {
    return end - start;
}

Vector2 LineSegment2::CalcDirection() const noexcept {
    return (end - start).GetNormalize();
}

Vector2 LineSegment2::CalcPositiveNormal() const noexcept {
    Vector2 dir = CalcDirection();
    dir.Rotate90Degrees();
    return dir;
}

Vector2 LineSegment2::CalcNegativeNormal() const noexcept {
    Vector2 dir = CalcDirection();
    dir.RotateNegative90Degrees();
    return dir;
}

LineSegment2 LineSegment2::operator+(const Vector2& translation) const noexcept {
    return LineSegment2(start + translation, end + translation);
}

LineSegment2 LineSegment2::operator-(const Vector2& antiTranslation) const noexcept {
    return LineSegment2(start - antiTranslation, end - antiTranslation);
}

LineSegment2& LineSegment2::operator-=(const Vector2& antiTranslation) noexcept {
    start -= antiTranslation;
    end -= antiTranslation;
    return *this;
}

LineSegment2& LineSegment2::operator+=(const Vector2& translation) noexcept {
    start += translation;
    end += translation;
    return *this;
}

void LineSegment2::SetAngle(float angleDegrees) noexcept {
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
