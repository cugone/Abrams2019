#pragma once

#include <string>

class Vector3;
class IntVector2;

class Vector2 {
public:

    static const Vector2 ONE;
    static const Vector2 ZERO;
    static const Vector2 X_AXIS;
    static const Vector2 Y_AXIS;
    static const Vector2 XY_AXIS;
    static const Vector2 YX_AXIS;

    Vector2() = default;
    Vector2(const Vector2& rhs) = default;
    Vector2(Vector2&& rhs) = default;
    Vector2& operator=(const Vector2& rhs) = default;
    Vector2& operator=(Vector2&& rhs) = default;
    ~Vector2() = default;

    explicit Vector2(const std::string& value);
    explicit Vector2(float initialX, float initialY);
    explicit Vector2(const Vector3& rhs);
    explicit Vector2(const IntVector2& intvec2);

    Vector2 operator+(const Vector2& rhs) const;
    Vector2& operator+=(const Vector2& rhs);

    Vector2 operator-() const;
    Vector2 operator-(const Vector2& rhs) const;
    Vector2& operator-=(const Vector2& rhs);

    friend Vector2 operator*(float lhs, const Vector2& rhs);
    Vector2 operator*(float scalar) const;
    Vector2& operator*=(float scalar);
    Vector2 operator*(const Vector2& rhs) const;
    Vector2& operator*=(const Vector2& rhs);

    Vector2 operator/(float scalar) const;
    Vector2 operator/=(float scalar);
    Vector2 operator/(const Vector2& rhs) const;
    Vector2 operator/=(const Vector2& rhs);

    bool operator==(const Vector2& rhs) const;
    bool operator!=(const Vector2& rhs) const;

    friend std::ostream& operator<<(std::ostream& out_stream, const Vector2& v);
    friend std::istream& operator>>(std::istream& in_stream, Vector2& v);

    void GetXY(float& outX, float& outY) const;
    float* GetAsFloatArray();

    float CalcHeadingRadians() const;
    float CalcHeadingDegrees() const;
    float CalcLength() const;
    float CalcLengthSquared() const;


    void SetHeadingDegrees(float headingDegrees);
    void SetHeadingRadians(float headingRadians);

    void SetUnitLengthAndHeadingDegrees(float headingDegrees);
    void SetUnitLengthAndHeadingRadians(float headingRadians);
    float SetLength(float length);
    void SetLengthAndHeadingDegrees(float headingDegrees, float length);
    void SetLengthAndHeadingRadians(float headingRadians, float length);

    float Normalize();
    Vector2 GetNormalize() const;

    void Rotate90Degrees();
    void RotateNegative90Degrees();
    void RotateRadians(float radians);

    void SetXY(float newX, float newY);

    float x = 0.0f;
    float y = 0.0f;

    friend void swap(Vector2& a, Vector2& b) noexcept;

protected:
private:
};