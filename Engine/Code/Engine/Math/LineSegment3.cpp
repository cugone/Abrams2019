#include "Engine/Math/LineSegment3.hpp"

#include <cmath>

const LineSegment3 LineSegment3::UNIT_HORIZONTAL(0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
const LineSegment3 LineSegment3::UNIT_VERTICAL(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
const LineSegment3 LineSegment3::UNIT_DEPTH(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
const LineSegment3 LineSegment3::UNIT_CENTERED_HORIZONTAL(-0.5f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f);
const LineSegment3 LineSegment3::UNIT_CENTERED_VERTICAL(0.0f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f);
const LineSegment3 LineSegment3::UNIT_CENTERED_DEPTH(0.0f, 0.0f, -0.5f, 0.0f, 0.0f, 0.5f);

LineSegment3::LineSegment3(float startX, float startY, float startZ, float endX, float endY, float endZ)
    : start(startX, startY, startZ)
    , end(endX, endY, endZ)
{
    /* DO NOTHING */
}

LineSegment3::LineSegment3(const Vector3& startPosition, const Vector3& endPosition)
    : start(startPosition)
    , end(endPosition)
{
    /* DO NOTHING */
}

LineSegment3::LineSegment3(const Vector3& startPosition, const Vector3& direction, float length)
    : start(startPosition)
    , end(startPosition + (direction.GetNormalize() * length))
{
    /* DO NOTHING */
}

void LineSegment3::SetLengthFromStart(float length) {
    end = (end - start).GetNormalize() * length;
}

void LineSegment3::SetLengthFromCenter(float length) {
    Vector3 center = CalcCenter();
    float half_length = length * 0.5f;
    start = (start - center).GetNormalize() * half_length;
    end = (end - center).GetNormalize() * half_length;
}

void LineSegment3::SetLengthFromEnd(float length) {
    start = (start - end).GetNormalize() * length;
}

Vector3 LineSegment3::CalcCenter() const {
    return start + (end - start) * 0.5f;
}

float LineSegment3::CalcLength() const {
    return (end - start).CalcLength();
}

float LineSegment3::CalcLengthSquared() const {
    return (end - start).CalcLengthSquared();
}

void LineSegment3::SetStartEndPositions(const Vector3& startPosition, const Vector3& endPosition) {
    start = startPosition;
    end = endPosition;
}

void LineSegment3::Translate(const Vector3& translation) {
    start += translation;
    end += translation;
}

Vector3 LineSegment3::CalcDisplacement() const {
    return (end - start);
}

Vector3 LineSegment3::CalcDirection() const {
    return (end - start).GetNormalize();
}

LineSegment3 LineSegment3::operator+(const Vector3& translation) const {
    return LineSegment3(start + translation, end + translation);
}

LineSegment3 LineSegment3::operator-(const Vector3& antiTranslation) const {
    return LineSegment3(start - antiTranslation, end - antiTranslation);
}

LineSegment3& LineSegment3::operator-=(const Vector3& antiTranslation) {
    start -= antiTranslation;
    end -= antiTranslation;
    return *this;
}

LineSegment3& LineSegment3::operator+=(const Vector3& translation) {
    start += translation;
    end += translation;
    return *this;
}
