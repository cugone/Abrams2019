#pragma once

#include "Engine/Math/Vector3.hpp"

class LineSegment3 {
public:

    Vector3 start = Vector3::ZERO;
    Vector3 end = Vector3::ZERO;

    static const LineSegment3 UNIT_HORIZONTAL;
    static const LineSegment3 UNIT_VERTICAL;
    static const LineSegment3 UNIT_DEPTH;
    static const LineSegment3 UNIT_CENTERED_HORIZONTAL;
    static const LineSegment3 UNIT_CENTERED_VERTICAL;
    static const LineSegment3 UNIT_CENTERED_DEPTH;

    LineSegment3() = default;
    LineSegment3(const LineSegment3& rhs) = default;
    LineSegment3(LineSegment3&& rhs) = default;
    ~LineSegment3() = default;
    LineSegment3& operator=(LineSegment3&& rhs) = default;
    LineSegment3& operator=(const LineSegment3& rhs) = default;

    explicit LineSegment3(float startX, float startY, float startZ, float endX, float endY, float endZ);
    explicit LineSegment3(const Vector3& startPosition, const Vector3& endPosition);
    explicit LineSegment3(const Vector3& startPosition, const Vector3& direction, float length);

    void SetLengthFromStart(float length);
    void SetLengthFromCenter(float length);
    void SetLengthFromEnd(float length);

    Vector3 CalcCenter() const;

    float CalcLength() const;
    float CalcLengthSquared() const;

    void SetStartEndPositions(const Vector3& startPosition, const Vector3& endPosition);

    void Translate(const Vector3& translation);

    Vector3 CalcDisplacement() const;
    Vector3 CalcDirection() const;

    LineSegment3 operator+(const Vector3& translation) const;
    LineSegment3 operator-(const Vector3& antiTranslation) const;
    LineSegment3& operator+=(const Vector3& translation);
    LineSegment3& operator-=(const Vector3& antiTranslation);

protected:
private:

    friend class Capsule3;
};