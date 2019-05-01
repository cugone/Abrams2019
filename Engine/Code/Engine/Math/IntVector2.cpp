#include "Engine/Math/IntVector2.hpp"

#include "Engine/Core/StringUtils.hpp"

#include "Engine/Math/IntVector3.hpp"
#include "Engine/Math/Vector2.hpp"

#include <cmath>
#include <sstream>

const IntVector2 IntVector2::ZERO(0, 0);
const IntVector2 IntVector2::ONE(1, 1);
const IntVector2 IntVector2::X_AXIS(1, 0);
const IntVector2 IntVector2::Y_AXIS(0, 1);
const IntVector2 IntVector2::XY_AXIS(1, 1);
const IntVector2 IntVector2::YX_AXIS(1, 1);

IntVector2::IntVector2(int initialX, int initialY)
    : x(initialX)
    , y(initialY)
{
    /* DO NOTHING */
}

IntVector2::IntVector2(const Vector2& v2)
    : x(static_cast<int>(std::floor(v2.x)))
    , y(static_cast<int>(std::floor(v2.y)))
{
    /* DO NOTHING */
}

IntVector2::IntVector2(const IntVector3& iv3)
    : x(iv3.x)
    , y(iv3.y)
{
    /* DO NOTHING */
}

IntVector2::IntVector2(const std::string& value)
    : x(0)
    , y(0)
{
    if(value[0] == '[') {
        if(value.back() == ']') {
            std::string contents_str = value.substr(1, value.size() - 1);
            auto values = StringUtils::Split(contents_str);
            auto s = values.size();
            for(std::size_t i = 0; i < s; ++i) {
                switch(i) {
                    case 0: x = std::stoi(values[i]); break;
                    case 1: y = std::stoi(values[i]); break;
                    default: break;
                }
            }
        }
    }
}

IntVector2 IntVector2::operator-() const {
    return IntVector2(-x, -y);
}

IntVector2 IntVector2::operator+(const IntVector2& rhs) const {
    return IntVector2(x + rhs.x, y + rhs.y);
}

IntVector2& IntVector2::operator+=(const IntVector2& rhs) {
    x += rhs.x;
    y += rhs.y;
    return *this;
}

IntVector2 IntVector2::operator-(const IntVector2& rhs) const {
    return IntVector2(x - rhs.x, y - rhs.y);
}

IntVector2& IntVector2::operator-=(const IntVector2& rhs) {
    x -= rhs.x;
    y -= rhs.y;
    return *this;
}

std::ostream& operator<<(std::ostream& out_stream, const IntVector2& v) {
    out_stream << '[' << v.x << ',' << v.y << ']';
    return out_stream;
}

std::istream& operator>>(std::istream& in_stream, IntVector2& v) {
    int x = 0;
    int y = 0;

    in_stream.ignore(); //[
    in_stream >> x;
    in_stream.ignore(); //,
    in_stream >> y;
    in_stream.ignore(); //]

    v.x = x;
    v.y = y;

    return in_stream;
}
IntVector2 operator*(int lhs, const IntVector2& rhs) {
    return IntVector2(lhs * rhs.x, lhs * rhs.y);
}

IntVector2 IntVector2::operator*(const IntVector2& rhs) const {
    return IntVector2(x * rhs.x, y * rhs.y);
}

IntVector2& IntVector2::operator*=(const IntVector2& rhs) {
    x *= rhs.x;
    y *= rhs.y;
    return *this;
}

IntVector2 IntVector2::operator*(int scalar) const {
    return IntVector2(x * scalar, y * scalar);
}

IntVector2& IntVector2::operator*=(int scalar) {
    x *= scalar;
    y *= scalar;
    return *this;
}

IntVector2 IntVector2::operator*(float scalar) const {
    int nx = static_cast<int>(std::floor(static_cast<float>(x) * scalar));
    int ny = static_cast<int>(std::floor(static_cast<float>(y) * scalar));
    return IntVector2(nx, ny);
}

IntVector2& IntVector2::operator*=(float scalar) {
    x = static_cast<int>(std::floor(static_cast<float>(x) * scalar));
    y = static_cast<int>(std::floor(static_cast<float>(y) * scalar));
    return *this;
}

IntVector2 IntVector2::operator/(const IntVector2& rhs) const {
    return IntVector2(x / rhs.x, y / rhs.y);
}

IntVector2& IntVector2::operator/=(const IntVector2& rhs) {
    x /= rhs.x;
    y /= rhs.y;
    return *this;
}

IntVector2 IntVector2::operator/(int scalar) const {
    return IntVector2(x / scalar, y / scalar);
}

IntVector2& IntVector2::operator/=(int scalar) {
    x /= scalar;
    y /= scalar;
    return *this;
}

IntVector2 IntVector2::operator/(float scalar) const {
    int nx = static_cast<int>(std::floor(static_cast<float>(x) / scalar));
    int ny = static_cast<int>(std::floor(static_cast<float>(y) / scalar));
    return IntVector2(nx, ny);
}

IntVector2& IntVector2::operator/=(float scalar) {
    x = static_cast<int>(std::floor(static_cast<float>(x) / scalar));
    y = static_cast<int>(std::floor(static_cast<float>(y) / scalar));
    return *this;
}

void IntVector2::SetXY(int newX, int newY) {
    x = newX;
    y = newY;
}

std::pair<int,int> IntVector2::GetXY() const {
    return std::make_pair(x, y);
}

bool IntVector2::operator!=(const IntVector2& rhs) const {
    return !(*this == rhs);
}

bool IntVector2::operator==(const IntVector2& rhs) const {
    return x == rhs.x && y == rhs.y;
}

bool IntVector2::operator<(const IntVector2& rhs) const {
    if(x < rhs.x) return true;
    if(rhs.x < x) return false;
    if(y < rhs.y) return true;
    return false;
}

bool IntVector2::operator>=(const IntVector2& rhs) const {
    return !(*this < rhs);
}

bool IntVector2::operator>(const IntVector2& rhs) const {
    return rhs < *this;
}

bool IntVector2::operator<=(const IntVector2& rhs) const {
    return !(*this > rhs);
}
