#pragma once

#include "Engine/Math/LineSegment3.hpp"
#include "Engine/Math/Vector3.hpp"

class Capsule3 {
public:

    static const Capsule3 UNIT_HORIZONTAL;
    static const Capsule3 UNIT_VERTICAL;
    static const Capsule3 UNIT_DEPTH;
    static const Capsule3 UNIT_CENTERED_HORIZONTAL;
    static const Capsule3 UNIT_CENTERED_VERTICAL;
    static const Capsule3 UNIT_CENTERED_DEPTH;

    LineSegment3 line{};
    float radius = 0.0f;

    Capsule3() = default;
    Capsule3(const Capsule3& rhs) = default;
    Capsule3(Capsule3&& rhs) = default;
    Capsule3& operator=(const Capsule3& rhs) = default;
    Capsule3& operator=(Capsule3&& rhs) = default;
    ~Capsule3() = default;

    explicit Capsule3(const LineSegment3& line, float radius) noexcept;
    explicit Capsule3(float startX, float startY, float startZ, float endX, float endY, float endZ, float radius) noexcept;
    explicit Capsule3(const Vector3& start_position, const Vector3& end_position, float radius) noexcept;
    explicit Capsule3(const Vector3& start_position, const Vector3& direction, float length, float radius) noexcept;

    void SetLengthFromStart(float length) noexcept;
    void SetLengthFromCenter(float length) noexcept;
    void SetLengthFromEnd(float length) noexcept;

    Vector3 CalcCenter() const noexcept;

    float CalcLength() const noexcept;
    float CalcLengthSquared() const noexcept;

    void SetStartEndPositions(const Vector3& start_position, const Vector3& end_position) noexcept;

    void Translate(const Vector3& translation) noexcept;

    Vector3 CalcDisplacement() const noexcept;
    Vector3 CalcDirection() const noexcept;

    Capsule3 operator+(const Vector3& translation) const noexcept;
    Capsule3 operator-(const Vector3& antiTranslation) const noexcept;

    Capsule3& operator+=(const Vector3& translation) noexcept;
    Capsule3& operator-=(const Vector3& antiTranslation) noexcept;

protected:
private:

};