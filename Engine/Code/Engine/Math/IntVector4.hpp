#pragma once

#include <string>
#include <tuple>

class IntVector2;
class IntVector3;
class Vector2;
class Vector3;
class Vector4;

class IntVector4 {
public:

    static const IntVector4 ZERO;
    static const IntVector4 ONE;
    static const IntVector4 X_AXIS;
    static const IntVector4 Y_AXIS;
    static const IntVector4 Z_AXIS;
    static const IntVector4 W_AXIS;
    static const IntVector4 XY_AXIS;
    static const IntVector4 XZ_AXIS;
    static const IntVector4 XW_AXIS;
    static const IntVector4 YX_AXIS;
    static const IntVector4 YZ_AXIS;
    static const IntVector4 YW_AXIS;
    static const IntVector4 ZX_AXIS;
    static const IntVector4 ZY_AXIS;
    static const IntVector4 ZW_AXIS;
    static const IntVector4 WX_AXIS;
    static const IntVector4 WY_AXIS;
    static const IntVector4 WZ_AXIS;
    static const IntVector4 XYZ_AXIS;
    static const IntVector4 XYW_AXIS;
    static const IntVector4 YXZ_AXIS;
    static const IntVector4 YZW_AXIS;
    static const IntVector4 WXY_AXIS;
    static const IntVector4 WXZ_AXIS;
    static const IntVector4 WYZ_AXIS;
    static const IntVector4 XYZW_AXIS;

    IntVector4() = default;
    ~IntVector4() = default;

    IntVector4(const IntVector4& rhs) = default;
    IntVector4(IntVector4&& rhs) = default;

    explicit IntVector4(const IntVector2& iv2, int initialZ, int initialW);
    explicit IntVector4(const Vector2& v2, int initialZ, int initialW);
    explicit IntVector4(const Vector2& xy, const Vector2& zw);
    explicit IntVector4(const IntVector2& xy, const IntVector2& zw);
    explicit IntVector4(int initialX, int initialY, int initialZ, int initialW);
    explicit IntVector4(const IntVector3& iv3, int initialW);
    explicit IntVector4(const Vector3& v3, int initialW);
    explicit IntVector4(const Vector4& rhs);
    explicit IntVector4(const std::string& value);

    IntVector4& operator=(const IntVector4& rhs) = default;
    IntVector4& operator=(IntVector4&& rhs) = default;

    bool operator==(const IntVector4& rhs);
    bool operator!=(const IntVector4& rhs);

    void SetXYZW(int newX, int newY, int newZ, int newW);
    std::tuple<int,int,int,int> GetXYZW() const;

    int x = 0;
    int y = 0;
    int z = 0;
    int w = 0;

protected:
private:

};