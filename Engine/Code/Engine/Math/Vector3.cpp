#include "Engine/Math/Vector3.hpp"

#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/IntVector3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Quaternion.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector4.hpp"

#include <cmath>
#include <sstream>

const Vector3 Vector3::ZERO(0.0f, 0.0f, 0.0f);
const Vector3 Vector3::X_AXIS(1.0f, 0.0f, 0.0f);
const Vector3 Vector3::Y_AXIS(0.0f, 1.0f, 0.0f);
const Vector3 Vector3::Z_AXIS(0.0f, 0.0f, 1.0f);
const Vector3 Vector3::XY_AXIS(1.0f, 1.0f, 0.0f);
const Vector3 Vector3::XZ_AXIS(1.0f, 0.0f, 1.0f);
const Vector3 Vector3::YZ_AXIS(0.0f, 1.0f, 1.0f);
const Vector3 Vector3::ONE(1.0f, 1.0f, 1.0f);

Vector3::Vector3(float initialX, float initialY, float initialZ) noexcept
: x(initialX)
, y(initialY)
, z(initialZ) {
    /* DO NOTHING */
}

Vector3::Vector3(const Vector2& xy, float initialZ) noexcept
: x(xy.x)
, y(xy.y)
, z(initialZ) {
    /* DO NOTHING */
}

Vector3::Vector3(const Vector2& vec2) noexcept
: x(vec2.x)
, y(vec2.y)
, z(0.0f) {
    /* DO NOTHING */
}

Vector3::Vector3(const Vector4& vec4) noexcept
: x(vec4.x)
, y(vec4.y)
, z(vec4.z) {
    /* DO NOTHING */
}

Vector3::Vector3(const Quaternion& q) noexcept
: x(q.axis.x)
, y(q.axis.y)
, z(q.axis.z) {
    Normalize();
}

Vector3::Vector3(const std::string& value) noexcept
: x(0.0f)
, y(0.0f)
, z(0.0f) {
    if(value[0] == '[') {
        if(value.back() == ']') {
            std::string contents_str = value.substr(1, value.size() - 1);
            auto values = StringUtils::Split(contents_str);
            auto s = values.size();
            for(std::size_t i = 0; i < s; ++i) {
                switch(i) {
                case 0: x = std::stof(values[i]); break;
                case 1: y = std::stof(values[i]); break;
                case 2: z = std::stof(values[i]); break;
                default: break;
                }
            }
        }
    }
}

Vector3::Vector3(const IntVector3& intvec3) noexcept
: x(static_cast<float>(intvec3.x))
, y(static_cast<float>(intvec3.y))
, z(static_cast<float>(intvec3.z)) {
    /* DO NOTHING */
}

Vector3 Vector3::operator+(const Vector3& rhs) const noexcept {
    return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
}

Vector3& Vector3::operator+=(const Vector3& rhs) noexcept {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
}

Vector3 Vector3::operator-(const Vector3& rhs) const noexcept {
    return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
}

Vector3& Vector3::operator-=(const Vector3& rhs) noexcept {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
}

Vector3 Vector3::operator-() const noexcept {
    return Vector3(-x, -y, -z);
}

Vector3 Vector3::operator*(const Vector3& rhs) const noexcept {
    return Vector3(x * rhs.x, y * rhs.y, z * rhs.z);
}

Vector3 operator*(float lhs, const Vector3& rhs) noexcept {
    return Vector3(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z);
}

Vector3 Vector3::operator*(float scalar) const noexcept {
    return Vector3(x * scalar, y * scalar, z * scalar);
}

Vector3& Vector3::operator*=(float scalar) noexcept {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

Vector3& Vector3::operator*=(const Vector3& rhs) noexcept {
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;
    return *this;
}

Vector3 operator/(float lhs, const Vector3& v) noexcept {
    return Vector3(lhs / v.x, lhs / v.y, lhs / v.z);
}

Vector3 Vector3::operator/(float scalar) const noexcept {
    return Vector3(x / scalar, y / scalar, z / scalar);
}

Vector3 Vector3::operator/=(float scalar) noexcept {
    x /= scalar;
    y /= scalar;
    z /= scalar;
    return *this;
}

Vector3 Vector3::operator/(const Vector3& rhs) const noexcept {
    return Vector3(x / rhs.x, y / rhs.y, z / rhs.z);
}

Vector3 Vector3::operator/=(const Vector3& rhs) noexcept {
    x /= rhs.x;
    y /= rhs.y;
    z /= rhs.z;
    return *this;
}

bool Vector3::operator==(const Vector3& rhs) const noexcept {
    return x == rhs.x && y == rhs.y && z == rhs.z;
}

bool Vector3::operator!=(const Vector3& rhs) const noexcept {
    return !(*this == rhs);
}

std::ostream& operator<<(std::ostream& out_stream, const Vector3& v) noexcept {
    out_stream << '[' << v.x << ',' << v.y << ',' << v.z << ']';
    return out_stream;
}

std::istream& operator>>(std::istream& in_stream, Vector3& v) noexcept {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

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

void Vector3::GetXYZ(float& outX, float& outY, float& outZ) const noexcept {
    outX = x;
    outY = y;
    outZ = z;
}

Vector3 Vector3::GetXYZ() const noexcept {
    return Vector3{x, y, z};
}

Vector2 Vector3::GetXY() const noexcept {
    return Vector2{x, y};
}

float* Vector3::GetAsFloatArray() noexcept {
    return &x;
}

float Vector3::CalcLength() const noexcept {
    return std::sqrt(CalcLengthSquared());
}

float Vector3::CalcLengthSquared() const noexcept {
    return x * x + y * y + z * z;
}

float Vector3::Normalize() noexcept {
    float length = CalcLength();
    if(length > 0.0f) {
        float inv_length = 1.0f / length;
        x *= inv_length;
        y *= inv_length;
        z *= inv_length;
        return length;
    }
    return 0.0f;
}

Vector3 Vector3::GetNormalize() const noexcept {
    float length = CalcLength();
    if(length > 0.0f) {
        float inv_length = 1.0f / length;
        return Vector3(x * inv_length, y * inv_length, z * inv_length);
    }
    return Vector3::ZERO;
}

void Vector3::SetXYZ(float newX, float newY, float newZ) noexcept {
    x = newX;
    y = newY;
    z = newZ;
}

void swap(Vector3& a, Vector3& b) noexcept {
    std::swap(a.x, b.x);
    std::swap(a.y, b.y);
    std::swap(a.z, b.z);
}

std::string StringUtils::to_string(const Vector3& v) noexcept {
    std::ostringstream ss;
    ss << '[' << v.x << ',' << v.y << ',' << v.z << ']';
    return ss.str();
}
