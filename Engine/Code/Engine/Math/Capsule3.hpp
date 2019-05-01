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

    explicit Capsule3(const LineSegment3& line, float radius);
    explicit Capsule3(float startX, float startY, float startZ, float endX, float endY, float endZ, float radius);
    explicit Capsule3(const Vector3& start_position, const Vector3& end_position, float radius);
    explicit Capsule3(const Vector3& start_position, const Vector3& direction, float length, float radius);

    void SetLengthFromStart(float length);
    void SetLengthFromCenter(float length);
    void SetLengthFromEnd(float length);

    Vector3 CalcCenter() const;

    float CalcLength() const;
    float CalcLengthSquared() const;

    void SetStartEndPositions(const Vector3& start_position, const Vector3& end_position);

    void Translate(const Vector3& translation);

    Vector3 CalcDisplacement() const;
    Vector3 CalcDirection() const;

    Capsule3 operator+(const Vector3& translation) const;
    Capsule3 operator-(const Vector3& antiTranslation) const;

    Capsule3& operator+=(const Vector3& translation);
    Capsule3& operator-=(const Vector3& antiTranslation);

protected:
private:

};