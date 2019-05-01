#pragma once

#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Math/Vector2.hpp"

class Capsule2 {
public:

    static const Capsule2 UNIT_HORIZONTAL;
    static const Capsule2 UNIT_VERTICAL;
    static const Capsule2 UNIT_CENTERED_HORIZONTAL;
    static const Capsule2 UNIT_CENTERED_VERTICAL;

    LineSegment2 line{};
    float radius = 0.0f;

    Capsule2() = default;
    Capsule2(const Capsule2& rhs) = default;
    Capsule2(Capsule2&& rhs) = default;
    Capsule2& operator=(const Capsule2& rhs) = default;
    Capsule2& operator=(Capsule2&& rhs) = default;
    ~Capsule2() = default;

    explicit Capsule2(const LineSegment2& line, float radius);
    explicit Capsule2(float startX, float startY, float endX, float endY, float radius);
    explicit Capsule2(const Vector2& start_position, const Vector2& end_position, float radius);
    explicit Capsule2(const Vector2& start_position, const Vector2& direction, float length, float radius);
    explicit Capsule2(const Vector2& start_position, float angle_degrees, float length, float radius);

    void SetLengthFromStart(float length);
    void SetLengthFromCenter(float length);
    void SetLengthFromEnd(float length);

    Vector2 CalcCenter() const;

    float CalcLength() const;
    float CalcLengthSquared() const;

    void SetDirectionFromStart(float angle_degrees);
    void SetDirectionFromCenter(float angle_degrees);
    void SetDirectionFromEnd(float angle_degrees);

    void SetStartEndPositions(const Vector2& start_position, const Vector2& end_position);

    void Translate(const Vector2& translation);
    void Rotate(float angle_degrees);
    void RotateStartPosition(float angle_degrees);
    void RotateEndPosition(float angle_degrees);

    void Rotate90Degrees();
    void RotateNegative90Degrees();
    void Rotate180Degrees();

    Vector2 CalcDisplacement() const;
    Vector2 CalcDirection() const;

    Vector2 CalcPositiveNormal() const;
    Vector2 CalcNegativeNormal() const;

    Capsule2 operator+(const Vector2& translation) const;
    Capsule2 operator-(const Vector2& antiTranslation) const;

    Capsule2& operator+=(const Vector2& translation);
    Capsule2& operator-=(const Vector2& antiTranslation);

protected:
private:
    void SetAngle(float angle_degrees);
};