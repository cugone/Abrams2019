#pragma once

#include "Engine/Math/Vector3.hpp"

#include <string>

class Matrix4;

class Quaternion {
public:
    static Quaternion I;
    float w = 1.0f;
    Vector3 axis = Vector3::ZERO;

    static Quaternion GetIdentity() noexcept;
    static Quaternion CreateRealQuaternion(float scalar) noexcept;
    static Quaternion CreatePureQuaternion(const Vector3& v) noexcept;
    static Quaternion CreateFromAxisAngle(const Vector3& axis, float degreesAngle) noexcept;
    static Quaternion CreateFromEulerAnglesDegrees(float pitch, float yaw, float roll) noexcept;
    static Quaternion CreateFromEulerAnglesRadians(float pitch, float yaw, float roll) noexcept;
    static Quaternion CreateFromEulerAngles(float pitch, float yaw, float roll, bool degrees) noexcept;

    Quaternion() = default;
    explicit Quaternion(const std::string& value) noexcept;
    explicit Quaternion(const Matrix4& mat) noexcept;
    explicit Quaternion(const Vector3& rotations) noexcept;
    Quaternion(const Quaternion& other) = default;
    Quaternion& operator=(const Quaternion& rhs) = default;
    ~Quaternion() = default;

    explicit Quaternion(float initialScalar, const Vector3& initialAxis) noexcept;
    explicit Quaternion(float initialW, float initialX, float initialY, float initialZ) noexcept;

    Quaternion operator+(const Quaternion& rhs) const noexcept;
    Quaternion& operator+=(const Quaternion& rhs) noexcept;

    Quaternion operator-(const Quaternion& rhs) const noexcept;
    Quaternion& operator-=(const Quaternion& rhs) noexcept;

    Quaternion operator*(const Quaternion& rhs) const noexcept;
    Quaternion& operator*=(const Quaternion& rhs) noexcept;

    Quaternion operator*(const Vector3& rhs) const noexcept;
    Quaternion& operator*=(const Vector3& rhs) noexcept;

    Quaternion operator*(float scalar) const noexcept;
    Quaternion& operator*=(float scalar) noexcept;

    Quaternion operator-() noexcept;

    bool operator==(const Quaternion& rhs) const noexcept;
    bool operator!=(const Quaternion& rhs) const noexcept;

    Vector4 CalcAxisAnglesDegrees() const noexcept;
    Vector4 CalcAxisAnglesRadians() const noexcept;
    Vector4 CalcAxisAngles(bool degrees) const noexcept;
    Vector3 CalcEulerAnglesDegrees() const noexcept;
    Vector3 CalcEulerAnglesRadians() const noexcept;
    Vector3 CalcEulerAngles(bool degrees) const noexcept;

    float CalcLength() const noexcept;
    float CalcLengthSquared() const noexcept;
    Quaternion CalcInverse() const noexcept;

    void Normalize() noexcept;
    Quaternion GetNormalize() const noexcept;

    void Conjugate() noexcept;
    Quaternion GetConjugate() const noexcept;

    void Inverse() noexcept;

protected:
private:
};

Quaternion operator*(float scalar, const Quaternion& rhs) noexcept;
Quaternion& operator*=(float scalar, Quaternion& rhs) noexcept;

Quaternion operator*(const Vector3& lhs, const Quaternion& rhs) noexcept;
Quaternion& operator*=(const Vector3& lhs, Quaternion& rhs) noexcept;

Quaternion Conjugate(const Quaternion& q) noexcept;
Quaternion Inverse(const Quaternion& q) noexcept;
