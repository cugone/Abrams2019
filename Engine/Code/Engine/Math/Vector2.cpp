#include "Engine/Math/Vector2.hpp"

#include "Engine/Core/StringUtils.hpp"

#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/MathUtils.hpp"

#include <cmath>
#include <sstream>

const Vector2 Vector2::ZERO(0.0f, 0.0f);
const Vector2 Vector2::X_AXIS(1.0f, 0.0f);
const Vector2 Vector2::Y_AXIS(0.0f, 1.0f);
const Vector2 Vector2::ONE(1.0f, 1.0);
const Vector2 Vector2::XY_AXIS(1.0f, 1.0);
const Vector2 Vector2::YX_AXIS(1.0f, 1.0);

Vector2::Vector2(float initialX, float initialY)
: x(initialX)
, y(initialY)
{
    /* DO NOTHING */
}

Vector2::Vector2(const Vector3& rhs)
    : x(rhs.x)
    , y(rhs.y)
{
    /* DO NOTHING */
}

Vector2::Vector2(const std::string& value)
    : x(0.0f)
    , y(0.0f)
{
    if(value[0] == '[') {
        if(value.back() == ']') {
            std::string contents_str = value.substr(1, value.size() - 1);
            auto values = StringUtils::Split(contents_str);
            auto s = values.size();
            for(std::size_t i = 0; i < s; ++i) {
                switch(i) {
                    case 0: x = std::stof(values[i]); break;
                    case 1: y = std::stof(values[i]); break;
                    default: break;
                }
            }
        }
    }
}


Vector2::Vector2(const IntVector2& intvec2)
    : x(static_cast<float>(intvec2.x))
    , y(static_cast<float>(intvec2.y))
{
    /* DO NOTHING */
}

Vector2 Vector2::operator+(const Vector2& rhs) const {
    return Vector2(x + rhs.x, y + rhs.y);
}

Vector2& Vector2::operator+=(const Vector2& rhs) {
    x += rhs.x;
    y += rhs.y;
    return *this;
}

Vector2 Vector2::operator-(const Vector2& rhs) const {
    return Vector2(x - rhs.x, y - rhs.y);
}

Vector2& Vector2::operator-=(const Vector2& rhs) {
    x -= rhs.x;
    y -= rhs.y;
    return *this;
}

Vector2 Vector2::operator-() const {
    return Vector2(-x, -y);
}

Vector2 Vector2::operator*(const Vector2& rhs) const {
    return Vector2(x * rhs.x, y * rhs.y);
}

Vector2 operator*(float lhs, const Vector2& rhs) {
    return Vector2(lhs * rhs.x, lhs * rhs.y);
}

Vector2 Vector2::operator*(float scalar) const {
    return Vector2(x * scalar, y * scalar);
}

Vector2& Vector2::operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    return *this;
}

Vector2& Vector2::operator*=(const Vector2& rhs) {
    x *= rhs.x;
    y *= rhs.y;
    return *this;
}

Vector2 Vector2::operator/(float scalar) const {
    return Vector2(x / scalar, y / scalar);
}

Vector2 Vector2::operator/=(float scalar) {
    x /= scalar;
    y /= scalar;
    return *this;
}

Vector2 Vector2::operator/(const Vector2& rhs) const {
    return Vector2(x / rhs.x, y / rhs.y);
}

Vector2 Vector2::operator/=(const Vector2& rhs) {
    x /= rhs.x;
    y /= rhs.y;
    return *this;
}

bool Vector2::operator==(const Vector2& rhs) const {
    return x == rhs.x && y == rhs.y;
}

bool Vector2::operator!=(const Vector2& rhs) const {
    return !(*this == rhs);
}


std::ostream& operator<<(std::ostream& out_stream, const Vector2& v) {
    out_stream << '[' << v.x << ',' << v.y << ']';
    return out_stream;
}

std::istream& operator>>(std::istream& in_stream, Vector2& v) {
    float x = 0.0f;
    float y = 0.0f;

    in_stream.ignore(); //[
    in_stream >> x;
    in_stream.ignore(); //,
    in_stream >> y;
    in_stream.ignore(); //]

    v.x = x;
    v.y = y;

    return in_stream;
}


void Vector2::GetXY(float& outX, float& outY) const {
    outX = x;
    outY = y;
}

float* Vector2::GetAsFloatArray() {
    return &x;
}

float Vector2::CalcHeadingRadians() const {
    return std::atan2(y, x);
}

float Vector2::CalcHeadingDegrees() const {
    return MathUtils::ConvertRadiansToDegrees(CalcHeadingRadians());
}

float Vector2::CalcLength() const {
    return std::sqrt(CalcLengthSquared());
}

float Vector2::CalcLengthSquared() const {
    return x * x + y * y;
}

void Vector2::SetHeadingDegrees(float headingDegrees) {
    SetHeadingRadians(MathUtils::ConvertDegreesToRadians(headingDegrees));
}

void Vector2::SetHeadingRadians(float headingRadians) {
    float R = CalcLength();
    float theta = headingRadians;
    x = R * std::cos(theta);
    y = R * std::sin(theta);
}

void Vector2::SetUnitLengthAndHeadingDegrees(float headingDegrees) {
    SetUnitLengthAndHeadingRadians(MathUtils::ConvertDegreesToRadians(headingDegrees));
}

void Vector2::SetUnitLengthAndHeadingRadians(float headingRadians) {
    Normalize();
    SetHeadingRadians(headingRadians);
}

float Vector2::SetLength(float length) {
    float R = CalcLength();
    float theta = CalcHeadingRadians();
    x = length * std::cos(theta);
    y = length * std::sin(theta);
    return R;    
}

void Vector2::SetLengthAndHeadingDegrees(float headingDegrees, float length) {
    SetLengthAndHeadingRadians(MathUtils::ConvertDegreesToRadians(headingDegrees), length);
}

void Vector2::SetLengthAndHeadingRadians(float headingRadians, float length) {
    SetLength(length);
    SetHeadingRadians(headingRadians);
}

void Vector2::RotateRadians(float radians) {
    float R = CalcLength();
    float old_angle = std::atan2(y, x);
    float new_angle = old_angle + radians;

    x = R * std::cos(new_angle);
    y = R * std::sin(new_angle);
}

float Vector2::Normalize() {
    float length = CalcLength();
    if(length > 0.0f) {
        float inv_length = 1.0f / length;
        x *= inv_length;
        y *= inv_length;
        return length;
    }
    return 0.0f;
}

Vector2 Vector2::GetNormalize() const {
    float length = CalcLength();
    if(length > 0.0f) {
        float inv_length = 1.0f / length;
        return Vector2(x * inv_length, y * inv_length);
    }
    return Vector2::ZERO;
}

void Vector2::Rotate90Degrees() {
    SetXY(-y, x);
}

void Vector2::RotateNegative90Degrees() {
    SetXY(y, -x);
}

void Vector2::SetXY(float newX, float newY) {
    x = newX;
    y = newY;
}


void swap(Vector2& a, Vector2& b) noexcept {
    std::swap(a.x, b.y);
    std::swap(a.y, b.y);
}