#pragma once

#include <string>

class Vector2;
class IntVector3;
class Vector4;
class Quaternion;

class Vector3 {
public:

    static const Vector3 ONE;
    static const Vector3 ZERO;
    static const Vector3 X_AXIS;
    static const Vector3 Y_AXIS;
    static const Vector3 Z_AXIS;
    static const Vector3 XY_AXIS;
    static const Vector3 XZ_AXIS;
    static const Vector3 YZ_AXIS;

    Vector3() noexcept = default;
    explicit Vector3(const std::string& value) noexcept;
    explicit Vector3(float initialX, float initialY, float initialZ) noexcept;
    explicit Vector3(const Vector2& vec2) noexcept;
    explicit Vector3(const IntVector3& intvec3) noexcept;
    explicit Vector3(const Vector2& xy, float initialZ) noexcept;
    explicit Vector3(const Vector4& vec4) noexcept;
    explicit Vector3(const Quaternion& q) noexcept;

    Vector3 operator+(const Vector3& rhs) const noexcept;
    Vector3& operator+=(const Vector3& rhs) noexcept;

    Vector3 operator-() const noexcept;
    Vector3 operator-(const Vector3& rhs) const noexcept;
    Vector3& operator-=(const Vector3& rhs) noexcept;

    friend Vector3 operator*(float lhs, const Vector3& rhs) noexcept;
    Vector3 operator*(float scalar) const noexcept;
    Vector3& operator*=(float scalar) noexcept;
    Vector3 operator*(const Vector3& rhs) const noexcept;
    Vector3& operator*=(const Vector3& rhs) noexcept;

    friend Vector3 operator/(float lhs, const Vector3& v) noexcept;
    Vector3 operator/(float scalar) const noexcept;
    Vector3 operator/=(float scalar) noexcept;
    Vector3 operator/(const Vector3& rhs) const noexcept;
    Vector3 operator/=(const Vector3& rhs) noexcept;

    bool operator==(const Vector3& rhs) const noexcept;
    bool operator!=(const Vector3& rhs) const noexcept;

    friend std::ostream& operator<<(std::ostream& out_stream, const Vector3& v) noexcept;
    friend std::istream& operator>>(std::istream& in_stream, Vector3& v) noexcept;

    void GetXYZ(float& outX, float& outY, float& outZ) const noexcept;
    Vector2 GetXY() const noexcept;
    Vector3 GetXYZ() const noexcept;
    float* GetAsFloatArray() noexcept;

    float CalcLength() const noexcept;
    float CalcLengthSquared() const noexcept;
    
    float Normalize() noexcept;
    Vector3 GetNormalize() const noexcept;

    void SetXYZ(float newX, float newY, float newZ) noexcept;

    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    friend void swap(Vector3& a, Vector3& b) noexcept;

protected:
private:
};

namespace StringUtils {
    std::string to_string(const Vector3& v) noexcept;
}