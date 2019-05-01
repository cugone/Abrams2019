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

    explicit IntVector2(int initialX, int initialY);
    explicit IntVector2(const Vector2& v2);
    explicit IntVector2(const IntVector3& iv3);
    explicit IntVector2(const std::string& value);

    IntVector2& operator=(const IntVector2& rhs) = default;
    IntVector2& operator=(IntVector2&& rhs) = default;
    
    IntVector2 operator+(const IntVector2& rhs) const;
    IntVector2& operator+=(const IntVector2& rhs);

    IntVector2 operator-() const;
    IntVector2 operator-(const IntVector2& rhs) const;
    IntVector2& operator-=(const IntVector2& rhs);

    friend IntVector2 operator*(int lhs, const IntVector2& rhs);
    IntVector2 operator*(const IntVector2& rhs) const;
    IntVector2& operator*=(const IntVector2& rhs);
    IntVector2 operator*(int scalar) const;
    IntVector2& operator*=(int scalar);
    IntVector2 operator*(float scalar) const;
    IntVector2& operator*=(float scalar);

    IntVector2 operator/(const IntVector2& rhs) const;
    IntVector2& operator/=(const IntVector2& rhs);
    IntVector2 operator/(int scalar) const;
    IntVector2& operator/=(int scalar);
    IntVector2 operator/(float scalar) const;
    IntVector2& operator/=(float scalar);

    bool operator==(const IntVector2& rhs) const;
    bool operator!=(const IntVector2& rhs) const;
    bool operator<(const IntVector2& rhs) const;
    bool operator>=(const IntVector2& rhs) const;
    bool operator>(const IntVector2& rhs) const;
    bool operator<=(const IntVector2& rhs) const;

    friend std::ostream& operator<<(std::ostream& out_stream, const IntVector2& v);
    friend std::istream& operator>>(std::istream& in_stream, IntVector2& v);

    void SetXY(int newX, int newY);
    std::pair<int, int> GetXY() const;

    int x = 0;
    int y = 0;

protected:
private:

};