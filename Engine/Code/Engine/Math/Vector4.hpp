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

    static Vector4 CalcHomogeneous(const Vector4& v);

    Vector4() = default;
    ~Vector4() = default;
    Vector4(const Vector4& rhs) = default;
    Vector4(Vector4&& rhs) = default;
    Vector4& operator=(const Vector4& rhs) = default;
    Vector4& operator=(Vector4&& rhs) = default;

    explicit Vector4(const std::string& value);
    explicit Vector4(const IntVector4& intvec4);
    explicit Vector4(const Vector3& xyz, float initialW);
    explicit Vector4(const Vector2& xy, float initialZ, float initialW);
    explicit Vector4(const Vector2& xy, const Vector2& zw);
    explicit Vector4(float initialX, float initialY, float initialZ, float initialW);

    bool operator==(const Vector4& rhs) const;
    bool operator!=(const Vector4& rhs) const;
    
    Vector4 operator+(const Vector4& rhs) const;
    Vector4 operator-(const Vector4& rhs) const;
    Vector4 operator*(const Vector4& rhs) const;
    Vector4 operator*(float scale) const;
    Vector4 operator/(const Vector4 rhs) const;
    Vector4 operator/(float inv_scale) const;

    friend Vector4 operator*(float lhs, const Vector4& rhs);
    Vector4& operator*=(float scale);
    Vector4& operator*=(const Vector4& rhs);
    Vector4& operator/=(const Vector4& rhs);
    Vector4& operator+=(const Vector4& rhs);
    Vector4& operator-=(const Vector4& rhs);

    Vector4 operator-() const;

    friend std::ostream& operator<<(std::ostream& out_stream, const Vector4& v);
    friend std::istream& operator>>(std::istream& in_stream, Vector4& v);

    Vector2 GetXY() const;
    Vector2 GetZW() const;

    void GetXYZ(float& out_x, float& out_y, float& out_z) const;
    void GetXYZW(float& out_x, float& out_y, float& out_z, float& out_w) const;
    void SetXYZ(float newX, float newY, float newZ);
    void SetXYZW(float newX, float newY, float newZ, float newW);

    float* GetAsFloatArray();

    float CalcLength3D() const;
    float CalcLength3DSquared() const;
    float CalcLength4D() const;
    float CalcLength4DSquared() const;
    void CalcHomogeneous();

    float Normalize4D();
    float Normalize3D();

    Vector4 GetNormalize4D() const;
    Vector4 GetNormalize3D() const;

    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 0.0f;

protected:
private:
};