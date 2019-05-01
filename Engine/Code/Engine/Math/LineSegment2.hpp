#pragma once

#include "Engine/Math/Vector2.hpp"

class LineSegment2 {
public:

    Vector2 start = Vector2::ZERO;
    Vector2 end = Vector2::ZERO;

    static const LineSegment2 UNIT_HORIZONTAL;
    static const LineSegment2 UNIT_VERTICAL;
    static const LineSegment2 UNIT_CENTERED_HORIZONTAL;
    static const LineSegment2 UNIT_CENTERED_VERTICAL;

    LineSegment2() = default;
    LineSegment2(const LineSegment2& rhs) = default;
    LineSegment2(LineSegment2&& rhs) = default;
    ~LineSegment2() = default;
    LineSegment2& operator=(LineSegment2&& rhs) = default;
    LineSegment2& operator=(const LineSegment2& rhs) = default;

    explicit LineSegment2(float startX, float startY, float endX, float endY);
    explicit LineSegment2(const Vector2& startPosition, const Vector2& endPosition);
    explicit LineSegment2(const Vector2& startPosition, const Vector2& direction, float length);
    explicit LineSegment2(const Vector2& startPosition, float angleDegrees, float length);

    void SetLengthFromStart(float length);
    void SetLengthFromCenter(float length);
    void SetLengthFromEnd(float length);

    Vector2 CalcCenter() const;

    float CalcLength() const;
    float CalcLengthSquared() const;

    void SetDirectionFromStart(float angleDegrees);
    void SetDirectionFromCenter(float angleDegrees);
    void SetDirectionFromEnd(float angleDegrees);

    void SetStartEndPositions(const Vector2& startPosition, const Vector2& endPosition);

    void Translate(const Vector2& translation);

    void Rotate(float angleDegrees);
    void RotateStartPosition(float angleDegrees);
    void RotateEndPosition(float angleDegrees);
    void Rotate90Degrees();
    void RotateNegative90Degrees();
    void Rotate180Degrees();

    Vector2 CalcDisplacement() const;
    Vector2 CalcDirection() const;
    Vector2 CalcPositiveNormal() const;
    Vector2 CalcNegativeNormal() const;

    LineSegment2 operator+(const Vector2& translation) const;
    LineSegment2 operator-(const Vector2& antiTranslation) const;
    LineSegment2& operator+=(const Vector2& translation);
    LineSegment2& operator-=(const Vector2& antiTranslation);

protected:
private:
    void SetAngle(float angleDegrees);

    friend class Capsule2;
};