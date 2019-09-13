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

    explicit LineSegment3(float startX, float startY, float startZ, float endX, float endY, float endZ) noexcept;
    explicit LineSegment3(const Vector3& startPosition, const Vector3& endPosition) noexcept;
    explicit LineSegment3(const Vector3& startPosition, const Vector3& direction, float length) noexcept;

    void SetLengthFromStart(float length) noexcept;
    void SetLengthFromCenter(float length) noexcept;
    void SetLengthFromEnd(float length) noexcept;

    Vector3 CalcCenter() const noexcept;

    float CalcLength() const noexcept;
    float CalcLengthSquared() const noexcept;

    void SetStartEndPositions(const Vector3& startPosition, const Vector3& endPosition) noexcept;

    void Translate(const Vector3& translation) noexcept;

    Vector3 CalcDisplacement() const noexcept;
    Vector3 CalcDirection() const noexcept;

    LineSegment3 operator+(const Vector3& translation) const noexcept;
    LineSegment3 operator-(const Vector3& antiTranslation) const noexcept;
    LineSegment3& operator+=(const Vector3& translation) noexcept;
    LineSegment3& operator-=(const Vector3& antiTranslation) noexcept;

protected:
private:

    friend class Capsule3;
};