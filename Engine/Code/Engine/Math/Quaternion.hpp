#pragma once

#include <string>

#include "Engine/Math/Vector3.hpp"

class Matrix4;

class Quaternion {
public:
    static Quaternion I;
    float w = 1.0f;
    Vector3 axis = Vector3::ZERO;

    static Quaternion GetIdentity();
    static Quaternion CreateRealQuaternion(float scalar);
    static Quaternion CreatePureQuaternion(const Vector3& v);
    static Quaternion CreateFromAxisAngle(const Vector3& axis, float degreesAngle);
    static Quaternion CreateFromEulerAnglesDegrees(float pitch, float yaw, float roll);
    static Quaternion CreateFromEulerAnglesRadians(float pitch, float yaw, float roll);
    static Quaternion CreateFromEulerAngles(float pitch, float yaw, float roll, bool degrees);

    Quaternion() = default;
    explicit Quaternion(const std::string& value);
    explicit Quaternion(const Matrix4& mat);
    explicit Quaternion(const Vector3& rotations);
    Quaternion(const Quaternion& other) = default;
    Quaternion& operator=(const Quaternion& rhs) = default;
    ~Quaternion();

    explicit Quaternion(float initialScalar, const Vector3& initialAxis);
    explicit Quaternion(float initialW, float initialX, float initialY, float initialZ);

    Quaternion operator+(const Quaternion& rhs) const;
    Quaternion& operator+=(const Quaternion& rhs);

    Quaternion operator-(const Quaternion& rhs) const;
    Quaternion& operator-=(const Quaternion& rhs);

    Quaternion operator*(const Quaternion& rhs) const;
    Quaternion& operator*=(const Quaternion& rhs);

    Quaternion operator*(const Vector3& rhs) const;
    Quaternion& operator*=(const Vector3& rhs);

    Quaternion operator*(float scalar) const;
    Quaternion& operator*=(float scalar);

    Quaternion operator-();

    bool operator==(const Quaternion& rhs) const;
    bool operator!=(const Quaternion& rhs) const;

    Vector4 CalcAxisAnglesDegrees() const;
    Vector4 CalcAxisAnglesRadians() const;
    Vector4 CalcAxisAngles(bool degrees) const;
    Vector3 CalcEulerAnglesDegrees() const;
    Vector3 CalcEulerAnglesRadians() const;
    Vector3 CalcEulerAngles(bool degrees) const;

    float CalcLength() const;
    float CalcLengthSquared() const;
    Quaternion CalcInverse() const;

    void Normalize();
    Quaternion GetNormalize() const;

    void Conjugate();
    Quaternion GetConjugate() const;

    void Inverse();

protected:
private:
};

Quaternion operator*(float scalar, const Quaternion& rhs);
Quaternion& operator*=(float scalar, Quaternion& rhs);

Quaternion operator*(const Vector3& lhs, const Quaternion& rhs);
Quaternion& operator*=(const Vector3& lhs, Quaternion& rhs);

Quaternion Conjugate(const Quaternion& q);
Quaternion Inverse(const Quaternion& q);
