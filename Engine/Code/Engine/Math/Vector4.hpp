#pragma once

#include <string>

class Vector2;
class Vector3;
class IntVector4;

class Vector4 {
public:
    static const Vector4 ZERO;
    static const Vector4 ONE;
    static const Vector4 ZERO_XYZ_ONE_W;
    static const Vector4 ONE_XYZ_ZERO_W;
    static const Vector4 X_AXIS;
    static const Vector4 XY_AXIS;
    static const Vector4 XZ_AXIS;
    static const Vector4 XW_AXIS;
    static const Vector4 Y_AXIS;
    static const Vector4 YX_AXIS;
    static const Vector4 YZ_AXIS;
    static const Vector4 YW_AXIS;
    static const Vector4 Z_AXIS;
    static const Vector4 ZX_AXIS;
    static const Vector4 ZY_AXIS;
    static const Vector4 ZW_AXIS;
    static const Vector4 W_AXIS;
    static const Vector4 WX_AXIS;
    static const Vector4 WY_AXIS;
    static const Vector4 WZ_AXIS;
    static const Vector4 XYZ_AXIS;
    static const Vector4 YZW_AXIS;
    static const Vector4 XZW_AXIS;
    static const Vector4 XYW_AXIS;

    static Vector4 CalcHomogeneous(const Vector4& v) noexcept;

    Vector4() noexcept = default;
    explicit Vector4(const std::string& value) noexcept;
    explicit Vector4(const IntVector4& intvec4) noexcept;
    explicit Vector4(const Vector3& xyz, float initialW) noexcept;
    explicit Vector4(const Vector2& xy, float initialZ, float initialW) noexcept;
    explicit Vector4(const Vector2& xy, const Vector2& zw) noexcept;
    explicit Vector4(float initialX, float initialY, float initialZ, float initialW) noexcept;

    bool operator==(const Vector4& rhs) const noexcept;
    bool operator!=(const Vector4& rhs) const noexcept;

    Vector4 operator+(const Vector4& rhs) const noexcept;
    Vector4 operator-(const Vector4& rhs) const noexcept;
    Vector4 operator*(const Vector4& rhs) const noexcept;
    Vector4 operator*(float scale) const noexcept;
    Vector4 operator/(const Vector4 rhs) const noexcept;
    Vector4 operator/(float inv_scale) const noexcept;

    friend Vector4 operator*(float lhs, const Vector4& rhs) noexcept;
    Vector4& operator*=(float scale) noexcept;
    Vector4& operator*=(const Vector4& rhs) noexcept;
    Vector4& operator/=(const Vector4& rhs) noexcept;
    Vector4& operator+=(const Vector4& rhs) noexcept;
    Vector4& operator-=(const Vector4& rhs) noexcept;

    Vector4 operator-() const noexcept;

    friend std::ostream& operator<<(std::ostream& out_stream, const Vector4& v) noexcept;
    friend std::istream& operator>>(std::istream& in_stream, Vector4& v) noexcept;

    Vector2 GetXY() const noexcept;
    Vector2 GetZW() const noexcept;

    void GetXYZ(float& out_x, float& out_y, float& out_z) const noexcept;
    void GetXYZW(float& out_x, float& out_y, float& out_z, float& out_w) const noexcept;
    void SetXYZ(float newX, float newY, float newZ) noexcept;
    void SetXYZW(float newX, float newY, float newZ, float newW) noexcept;

    float* GetAsFloatArray() noexcept;

    float CalcLength3D() const noexcept;
    float CalcLength3DSquared() const noexcept;
    float CalcLength4D() const noexcept;
    float CalcLength4DSquared() const noexcept;
    void CalcHomogeneous() noexcept;

    float Normalize4D() noexcept;
    float Normalize3D() noexcept;

    Vector4 GetNormalize4D() const noexcept;
    Vector4 GetNormalize3D() const noexcept;

    friend void swap(Vector4& a, Vector4& b) noexcept;

    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 0.0f;

protected:
private:
};

namespace StringUtils {
std::string to_string(const Vector4& v) noexcept;
}