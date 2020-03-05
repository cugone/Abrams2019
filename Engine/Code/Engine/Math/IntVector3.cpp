#include "Engine/Math/IntVector3.hpp"

#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"

#include <cmath>
#include <sstream>

const IntVector3 IntVector3::ZERO(0, 0, 0);
const IntVector3 IntVector3::ONE(1, 1, 1);
const IntVector3 IntVector3::X_AXIS(1, 0, 0);
const IntVector3 IntVector3::Y_AXIS(0, 1, 0);
const IntVector3 IntVector3::Z_AXIS(0, 0, 1);
const IntVector3 IntVector3::XY_AXIS(1, 1, 0);
const IntVector3 IntVector3::XZ_AXIS(1, 0, 1);
const IntVector3 IntVector3::YX_AXIS(1, 1, 0);
const IntVector3 IntVector3::YZ_AXIS(0, 1, 1);
const IntVector3 IntVector3::ZX_AXIS(1, 0, 1);
const IntVector3 IntVector3::ZY_AXIS(0, 1, 1);
const IntVector3 IntVector3::XYZ_AXIS(1, 1, 1);

IntVector3::IntVector3(int initialX, int initialY, int initialZ) noexcept
: x(initialX)
, y(initialY)
, z(initialZ) {
    /* DO NOTHING */
}

IntVector3::IntVector3(const IntVector2& iv2, int initialZ) noexcept
: x(iv2.x)
, y(iv2.y)
, z(initialZ) {
    /* DO NOTHING */
}

IntVector3::IntVector3(const Vector2& v2, int initialZ) noexcept
: x(static_cast<int>(std::floor(v2.x)))
, y(static_cast<int>(std::floor(v2.y)))
, z(initialZ) {
    /* DO NOTHING */
}

IntVector3::IntVector3(const Vector3& v3) noexcept
: x(static_cast<int>(std::floor(v3.x)))
, y(static_cast<int>(std::floor(v3.y)))
, z(static_cast<int>(std::floor(v3.z))) {
    /* DO NOTHING */
}

IntVector3::IntVector3(const std::string& value) noexcept
: x(0)
, y(0)
, z(0) {
    if(value[0] == '[') {
        if(value.back() == ']') {
            std::string contents_str = value.substr(1, value.size() - 1);
            auto values = StringUtils::Split(contents_str);
            auto s = values.size();
            for(std::size_t i = 0; i < s; ++i) {
                switch(i) {
                case 0: x = std::stoi(values[i]); break;
                case 1: y = std::stoi(values[i]); break;
                case 2: z = std::stoi(values[i]); break;
                default: break;
                }
            }
        }
    }
}

IntVector3 IntVector3::operator+(const IntVector3& rhs) const noexcept {
    return IntVector3(x + rhs.x, y + rhs.y, z + rhs.z);
}

IntVector3& IntVector3::operator+=(const IntVector3& rhs) noexcept {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
}

IntVector3 IntVector3::operator-() const noexcept {
    return IntVector3(-x, -y, -z);
}

IntVector3 IntVector3::operator-(const IntVector3& rhs) const noexcept {
    return IntVector3(x - rhs.x, y - rhs.y, z - rhs.z);
}

IntVector3& IntVector3::operator-=(const IntVector3& rhs) noexcept {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
}

IntVector3 IntVector3::operator*(const IntVector3& rhs) const noexcept {
    return IntVector3(x * rhs.x, y * rhs.y, z * rhs.z);
}

IntVector3& IntVector3::operator*=(const IntVector3& rhs) noexcept {
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;
    return *this;
}

IntVector3 IntVector3::operator*(int scalar) const noexcept {
    return IntVector3(x * scalar, y * scalar, z * scalar);
}

IntVector3& IntVector3::operator*=(int scalar) noexcept {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

IntVector3 IntVector3::operator*(float scalar) const noexcept {
    int nx = static_cast<int>(std::floor(static_cast<float>(x) * scalar));
    int ny = static_cast<int>(std::floor(static_cast<float>(y) * scalar));
    int nz = static_cast<int>(std::floor(static_cast<float>(z) * scalar));
    return IntVector3(nx, ny, nz);
}

IntVector3& IntVector3::operator*=(float scalar) noexcept {
    x = static_cast<int>(std::floor(static_cast<float>(x) * scalar));
    y = static_cast<int>(std::floor(static_cast<float>(y) * scalar));
    z = static_cast<int>(std::floor(static_cast<float>(z) * scalar));
    return *this;
}

IntVector3 IntVector3::operator/(const IntVector3& rhs) const noexcept {
    return IntVector3(x / rhs.x, y / rhs.y, z / rhs.z);
}

IntVector3& IntVector3::operator/=(const IntVector3& rhs) noexcept {
    x /= rhs.x;
    y /= rhs.y;
    z /= rhs.z;
    return *this;
}

IntVector3 IntVector3::operator/(int scalar) const noexcept {
    return IntVector3(x / scalar, y / scalar, z / scalar);
}

IntVector3& IntVector3::operator/=(int scalar) noexcept {
    x /= scalar;
    y /= scalar;
    z /= scalar;
    return *this;
}

IntVector3 IntVector3::operator/(float scalar) const noexcept {
    int nx = static_cast<int>(std::floor(static_cast<float>(x) / scalar));
    int ny = static_cast<int>(std::floor(static_cast<float>(y) / scalar));
    int nz = static_cast<int>(std::floor(static_cast<float>(z) / scalar));
    return IntVector3(nx, ny, nz);
}

IntVector3& IntVector3::operator/=(float scalar) noexcept {
    x = static_cast<int>(std::floor(static_cast<float>(x) / scalar));
    y = static_cast<int>(std::floor(static_cast<float>(y) / scalar));
    z = static_cast<int>(std::floor(static_cast<float>(z) / scalar));
    return *this;
}

bool IntVector3::operator!=(const IntVector3& rhs) const noexcept {
    return !(*this == rhs);
}

bool IntVector3::operator==(const IntVector3& rhs) const noexcept {
    return x == rhs.x && y == rhs.y && z == rhs.z;
}

bool IntVector3::operator<(const IntVector3& rhs) const noexcept {
    if(x < rhs.x)
        return true;
    if(rhs.x < x)
        return false;
    if(y < rhs.y)
        return true;
    if(rhs.y < y)
        return false;
    if(z < rhs.z)
        return true;
    return false;
}

bool IntVector3::operator>=(const IntVector3& rhs) const noexcept {
    return !(*this < rhs);
}

bool IntVector3::operator>(const IntVector3& rhs) const noexcept {
    return rhs < *this;
}

bool IntVector3::operator<=(const IntVector3& rhs) const noexcept {
    return !(*this > rhs);
}

std::ostream& operator<<(std::ostream& out_stream, const IntVector3& v) noexcept {
    out_stream << '[' << v.x << ',' << v.y << ',' << v.z << ']';
    return out_stream;
}

std::istream& operator>>(std::istream& in_stream, IntVector3& v) noexcept {
    int x = 0;
    int y = 0;
    int z = 0;

    in_stream.ignore(); //[
    in_stream >> x;
    in_stream.ignore(); //,
    in_stream >> y;
    in_stream.ignore(); //,
    in_stream >> z;
    in_stream.ignore(); //]

    v.x = x;
    v.y = y;
    v.z = z;

    return in_stream;
}

void IntVector3::SetXYZ(int newX, int newY, int newZ) noexcept {
    x = newX;
    y = newY;
    z = newZ;
}

std::tuple<int, int, int> IntVector3::GetXYZ() const noexcept {
    return std::make_tuple(x, y, z);
}
