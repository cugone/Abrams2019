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

    explicit Capsule2(const LineSegment2& line, float radius) noexcept;
    explicit Capsule2(float startX, float startY, float endX, float endY, float radius) noexcept;
    explicit Capsule2(const Vector2& start_position, const Vector2& end_position, float radius) noexcept;
    explicit Capsule2(const Vector2& start_position, const Vector2& direction, float length, float radius) noexcept;
    explicit Capsule2(const Vector2& start_position, float angle_degrees, float length, float radius) noexcept;

    void SetLengthFromStart(float length) noexcept;
    void SetLengthFromCenter(float length) noexcept;
    void SetLengthFromEnd(float length) noexcept;

    Vector2 CalcCenter() const noexcept;

    float CalcLength() const noexcept;
    float CalcLengthSquared() const noexcept;

    void SetDirectionFromStart(float angle_degrees) noexcept;
    void SetDirectionFromCenter(float angle_degrees) noexcept;
    void SetDirectionFromEnd(float angle_degrees) noexcept;

    void SetStartEndPositions(const Vector2& start_position, const Vector2& end_position) noexcept;

    void Translate(const Vector2& translation) noexcept;
    void Rotate(float angle_degrees) noexcept;
    void RotateStartPosition(float angle_degrees) noexcept;
    void RotateEndPosition(float angle_degrees) noexcept;

    void Rotate90Degrees() noexcept;
    void RotateNegative90Degrees() noexcept;
    void Rotate180Degrees() noexcept;

    Vector2 CalcDisplacement() const noexcept;
    Vector2 CalcDirection() const noexcept;

    Vector2 CalcPositiveNormal() const noexcept;
    Vector2 CalcNegativeNormal() const noexcept;

    Capsule2 operator+(const Vector2& translation) const noexcept;
    Capsule2 operator-(const Vector2& antiTranslation) const noexcept;

    Capsule2& operator+=(const Vector2& translation) noexcept;
    Capsule2& operator-=(const Vector2& antiTranslation) noexcept;

protected:
private:
    void SetAngle(float angle_degrees) noexcept;
};