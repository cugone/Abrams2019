#pragma once

#include <string>
#include <tuple>

class IntVector2;
class Vector2;
class Vector3;

class IntVector3 {
public:

    static const IntVector3 ZERO;
    static const IntVector3 ONE;
    static const IntVector3 X_AXIS;
    static const IntVector3 Y_AXIS;
    static const IntVector3 Z_AXIS;
    static const IntVector3 XY_AXIS;
    static const IntVector3 XZ_AXIS;
    static const IntVector3 YX_AXIS;
    static const IntVector3 YZ_AXIS;
    static const IntVector3 ZX_AXIS;
    static const IntVector3 ZY_AXIS;
    static const IntVector3 XYZ_AXIS;


    IntVector3() = default;
    ~IntVector3() = default;

    IntVector3(const IntVector3& rhs) = default;
    IntVector3(IntVector3&& rhs) = default;

    explicit IntVector3(const IntVector2& iv2, int initialZ) noexcept;
    explicit IntVector3(const Vector2& v2, int initialZ) noexcept;
    explicit IntVector3(int initialX, int initialY, int initialZ) noexcept;
    explicit IntVector3(const Vector3& v3) noexcept;
    explicit IntVector3(const std::string& value) noexcept;

    IntVector3& operator=(const IntVector3& rhs) = default;
    IntVector3& operator=(IntVector3&& rhs) = default;

    bool operator==(const IntVector3& rhs) const noexcept;
    bool operator!=(const IntVector3& rhs) const noexcept;

    void SetXYZ(int newX, int newY, int newZ) noexcept;
    std::tuple<int, int, int> GetXYZ() const noexcept;

    int x = 0;
    int y = 0;
    int z = 0;

protected:
private:

};