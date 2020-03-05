#pragma once

#include <string>
#include <utility>

class Vector2;
class IntVector3;

class IntVector2 {
public:
    static const IntVector2 ZERO;
    static const IntVector2 ONE;
    static const IntVector2 X_AXIS;
    static const IntVector2 Y_AXIS;
    static const IntVector2 XY_AXIS;
    static const IntVector2 YX_AXIS;

    IntVector2() = default;
    ~IntVector2() = default;

    IntVector2(const IntVector2& rhs) = default;
    IntVector2(IntVector2&& rhs) = default;

    explicit IntVector2(int initialX, int initialY) noexcept;
    explicit IntVector2(const Vector2& v2) noexcept;
    explicit IntVector2(const IntVector3& iv3) noexcept;
    explicit IntVector2(const std::string& value) noexcept;

    IntVector2& operator=(const IntVector2& rhs) = default;
    IntVector2& operator=(IntVector2&& rhs) = default;

    IntVector2 operator+(const IntVector2& rhs) const noexcept;
    IntVector2& operator+=(const IntVector2& rhs) noexcept;

    IntVector2 operator-() const noexcept;
    IntVector2 operator-(const IntVector2& rhs) const noexcept;
    IntVector2& operator-=(const IntVector2& rhs) noexcept;

    friend IntVector2 operator*(int lhs, const IntVector2& rhs) noexcept;
    IntVector2 operator*(const IntVector2& rhs) const noexcept;
    IntVector2& operator*=(const IntVector2& rhs) noexcept;
    IntVector2 operator*(int scalar) const noexcept;
    IntVector2& operator*=(int scalar) noexcept;
    IntVector2 operator*(float scalar) const noexcept;
    IntVector2& operator*=(float scalar) noexcept;

    IntVector2 operator/(const IntVector2& rhs) const noexcept;
    IntVector2& operator/=(const IntVector2& rhs) noexcept;
    IntVector2 operator/(int scalar) const noexcept;
    IntVector2& operator/=(int scalar) noexcept;
    IntVector2 operator/(float scalar) const noexcept;
    IntVector2& operator/=(float scalar) noexcept;

    bool operator==(const IntVector2& rhs) const noexcept;
    bool operator!=(const IntVector2& rhs) const noexcept;
    bool operator<(const IntVector2& rhs) const noexcept;
    bool operator>=(const IntVector2& rhs) const noexcept;
    bool operator>(const IntVector2& rhs) const noexcept;
    bool operator<=(const IntVector2& rhs) const noexcept;

    friend std::ostream& operator<<(std::ostream& out_stream, const IntVector2& v) noexcept;
    friend std::istream& operator>>(std::istream& in_stream, IntVector2& v) noexcept;

    void SetXY(int newX, int newY) noexcept;
    std::pair<int, int> GetXY() const noexcept;

    int x = 0;
    int y = 0;

protected:
private:
};