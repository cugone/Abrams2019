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


    Vector3() = default;
    Vector3(const Vector3& rhs) = default;
    Vector3(Vector3&& rhs) = default;
    Vector3& operator=(const Vector3& rhs) = default;
    Vector3& operator=(Vector3&& rhs) = default;
    ~Vector3() = default;

    explicit Vector3(const std::string& value);
    explicit Vector3(float initialX, float initialY, float initialZ);
    explicit Vector3(const Vector2& vec2);
    explicit Vector3(const IntVector3& intvec3);
    explicit Vector3(const Vector2& xy, float initialZ);
    explicit Vector3(const Vector4& vec4);
    explicit Vector3(const Quaternion& q);

    Vector3 operator+(const Vector3& rhs) const;
    Vector3& operator+=(const Vector3& rhs);

    Vector3 operator-() const;
    Vector3 operator-(const Vector3& rhs) const;
    Vector3& operator-=(const Vector3& rhs);

    friend Vector3 operator*(float lhs, const Vector3& rhs);
    Vector3 operator*(float scalar) const;
    Vector3& operator*=(float scalar);
    Vector3 operator*(const Vector3& rhs) const;
    Vector3& operator*=(const Vector3& rhs);

    friend Vector3 operator/(float lhs, const Vector3& v);
    Vector3 operator/(float scalar) const;
    Vector3 operator/=(float scalar);
    Vector3 operator/(const Vector3& rhs) const;
    Vector3 operator/=(const Vector3& rhs);

    bool operator==(const Vector3& rhs) const;
    bool operator!=(const Vector3& rhs) const;

    friend std::ostream& operator<<(std::ostream& out_stream, const Vector3& v);
    friend std::istream& operator>>(std::istream& in_stream, Vector3& v);

    void GetXYZ(float& outX, float& outY, float& outZ) const;
    Vector2 GetXY() const;
    Vector3 GetXYZ() const;
    float* GetAsFloatArray();

    float CalcLength() const;
    float CalcLengthSquared() const;
    
    float Normalize();
    Vector3 GetNormalize() const;

    void SetXYZ(float newX, float newY, float newZ);

    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

protected:
private:
};
