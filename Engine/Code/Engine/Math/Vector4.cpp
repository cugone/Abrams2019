#include "Engine/Math/Vector4.hpp"

#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/IntVector4.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"

#include <cmath>
#include <sstream>

namespace a2de {

    const Vector4 Vector4::ZERO(0.0f, 0.0f, 0.0f, 0.0f);
    const Vector4 Vector4::ONE(1.0f, 1.0f, 1.0f, 1.0f);
    const Vector4 Vector4::ZERO_XYZ_ONE_W(0.0f, 0.0f, 0.0f, 1.0f);
    const Vector4 Vector4::ONE_XYZ_ZERO_W(1.0f, 1.0f, 1.0f, 0.0f);
    const Vector4 Vector4::X_AXIS(1.0f, 0.0f, 0.0f, 0.0f);
    const Vector4 Vector4::XY_AXIS(1.0f, 1.0f, 0.0f, 0.0f);
    const Vector4 Vector4::XZ_AXIS(1.0f, 0.0f, 1.0f, 0.0f);
    const Vector4 Vector4::XW_AXIS(1.0f, 0.0f, 0.0f, 1.0f);
    const Vector4 Vector4::Y_AXIS(0.0f, 1.0f, 0.0f, 0.0f);
    const Vector4 Vector4::YX_AXIS(1.0f, 1.0f, 0.0f, 0.0f);
    const Vector4 Vector4::YZ_AXIS(0.0f, 1.0f, 1.0f, 0.0f);
    const Vector4 Vector4::YW_AXIS(0.0f, 1.0f, 0.0f, 1.0f);
    const Vector4 Vector4::Z_AXIS(0.0f, 0.0f, 1.0f, 0.0f);
    const Vector4 Vector4::ZX_AXIS(1.0f, 0.0f, 1.0f, 0.0f);
    const Vector4 Vector4::ZY_AXIS(0.0f, 1.0f, 1.0f, 0.0f);
    const Vector4 Vector4::ZW_AXIS(0.0f, 0.0f, 1.0f, 1.0f);
    const Vector4 Vector4::W_AXIS(0.0f, 0.0f, 0.0f, 1.0f);
    const Vector4 Vector4::WX_AXIS(1.0f, 0.0f, 0.0f, 1.0f);
    const Vector4 Vector4::WY_AXIS(0.0f, 1.0f, 0.0f, 1.0f);
    const Vector4 Vector4::WZ_AXIS(0.0f, 0.0f, 1.0f, 1.0f);
    const Vector4 Vector4::XYZ_AXIS(1.0f, 1.0f, 1.0f, 0.0f);
    const Vector4 Vector4::YZW_AXIS(0.0f, 1.0f, 1.0f, 1.0f);
    const Vector4 Vector4::XZW_AXIS(1.0f, 0.0f, 1.0f, 1.0f);
    const Vector4 Vector4::XYW_AXIS(1.0f, 1.0f, 0.0f, 1.0f);

    Vector4::Vector4(const Vector3& xyz, float initialW) noexcept
        : x(xyz.x)
        , y(xyz.y)
        , z(xyz.z)
        , w(initialW) {
        /* DO NOTHING */
    }

    Vector4::Vector4(const Vector2& xy, float initialZ, float initialW) noexcept
        : x(xy.x)
        , y(xy.y)
        , z(initialZ)
        , w(initialW) {
        /* DO NOTHING */
    }

    Vector4::Vector4(const Vector2& xy, const Vector2& zw) noexcept
        : x(xy.x)
        , y(xy.y)
        , z(zw.x)
        , w(zw.y) {
        /* DO NOTHING */
    }

    Vector4::Vector4(float initialX, float initialY, float initialZ, float initialW) noexcept
        : x(initialX)
        , y(initialY)
        , z(initialZ)
        , w(initialW) {
        /* DO NOTHING */
    }

    Vector4::Vector4(const std::string& value) noexcept
        : x(0.0f)
        , y(0.0f)
        , z(0.0f)
        , w(0.0f) {
        if(value[0] == '[') {
            if(value.back() == ']') {
                std::string contents_str = std::string{std::begin(value) + 1, std::end(value) - 1};
                const auto values = StringUtils::Split(contents_str);
                const auto s = values.size();
                for(std::size_t i = 0; i < s; ++i) {
                    switch(i) {
                    case 0: x = std::stof(values[i]); break;
                    case 1: y = std::stof(values[i]); break;
                    case 2: z = std::stof(values[i]); break;
                    case 3: w = std::stof(values[i]); break;
                    default: break;
                    }
                }
            }
        }
    }

    Vector4::Vector4(const IntVector4& intvec4) noexcept
        : x(static_cast<float>(intvec4.x))
        , y(static_cast<float>(intvec4.y))
        , z(static_cast<float>(intvec4.z))
        , w(static_cast<float>(intvec4.w)) {
        /* DO NOTHING */
    }

    Vector4& Vector4::operator+=(const Vector4& rhs) noexcept {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        w += rhs.w;
        return *this;
    }

    Vector4& Vector4::operator-=(const Vector4& rhs) noexcept {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        w -= rhs.w;
        return *this;
    }

    Vector4 Vector4::operator-(const Vector4& rhs) const noexcept {
        return Vector4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
    }

    Vector4 Vector4::operator-() const noexcept {
        return Vector4(-x, -y, -z, -w);
    }

    std::ostream& operator<<(std::ostream& out_stream, const Vector4& v) noexcept {
        out_stream << '[' << v.x << ',' << v.y << ',' << v.z << ',' << v.w << ']';
        return out_stream;
    }

    std::istream& operator>>(std::istream& in_stream, Vector4& v) noexcept {
        auto x = 0.0f;
        auto y = 0.0f;
        auto z = 0.0f;
        auto w = 0.0f;

        in_stream.ignore(); //[
        in_stream >> x;
        in_stream.ignore(); //,
        in_stream >> y;
        in_stream.ignore(); //,
        in_stream >> z;
        in_stream.ignore(); //,
        in_stream >> w;
        in_stream.ignore(); //]

        v.x = x;
        v.y = y;
        v.z = z;
        v.w = w;

        return in_stream;
    }

    Vector2 Vector4::GetXY() const noexcept {
        return Vector2(x, y);
    }

    Vector2 Vector4::GetZW() const noexcept {
        return Vector2(z, w);
    }

    void Vector4::GetXYZ(float& out_x, float& out_y, float& out_z) const noexcept {
        out_x = x;
        out_y = y;
        out_z = z;
    }

    void Vector4::GetXYZW(float& out_x, float& out_y, float& out_z, float& out_w) const noexcept {
        out_x = x;
        out_y = y;
        out_z = z;
        out_w = w;
    }

    void Vector4::SetXYZ(float newX, float newY, float newZ) noexcept {
        x = newX;
        y = newY;
        z = newZ;
    }

    void Vector4::SetXYZW(float newX, float newY, float newZ, float newW) noexcept {
        x = newX;
        y = newY;
        z = newZ;
        w = newW;
    }

    float* Vector4::GetAsFloatArray() noexcept {
        return &x;
    }

    float Vector4::CalcLength3D() const noexcept {
        return std::sqrt(CalcLength3DSquared());
    }

    float Vector4::CalcLength3DSquared() const noexcept {
        return x * x + y * y + z * z;
    }

    float Vector4::CalcLength4D() const noexcept {
        return std::sqrt(CalcLength4DSquared());
    }

    float Vector4::CalcLength4DSquared() const noexcept {
        return x * x + y * y + z * z + w * w;
    }

    Vector4 Vector4::operator*(const Vector4& rhs) const noexcept {
        return Vector4(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w);
    }

    Vector4 operator*(float lhs, const Vector4& rhs) noexcept {
        return Vector4(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z, lhs * rhs.w);
    }
    Vector4 Vector4::operator*(float scale) const noexcept {
        return Vector4(x * scale, y * scale, z * scale, w * scale);
    }

    Vector4& Vector4::operator*=(float scale) noexcept {
        x *= scale;
        y *= scale;
        z *= scale;
        w *= scale;
        return *this;
    }

    Vector4& Vector4::operator*=(const Vector4& rhs) noexcept {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;
        w *= rhs.w;
        return *this;
    }

    Vector4& Vector4::operator/=(const Vector4& rhs) noexcept {
        x /= rhs.x;
        y /= rhs.y;
        z /= rhs.z;
        w /= rhs.w;
        return *this;
    }

    Vector4 Vector4::operator/(const Vector4 rhs) const noexcept {
        return Vector4(x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w);
    }

    Vector4 Vector4::operator/(float inv_scale) const noexcept {
        return Vector4(x / inv_scale, y / inv_scale, z / inv_scale, w / inv_scale);
    }

    Vector4 Vector4::CalcHomogeneous(const Vector4& v) noexcept {
        return std::fabs(v.w - 0.0f) < 0.0001f == false ? v / v.w : v;
    }

    void Vector4::CalcHomogeneous() noexcept {
        if(std::fabs(w - 0.0f) < 0.0001f == false) {
            x /= w;
            y /= w;
            z /= w;
            w = 1.0f;
        }
    }

    float Vector4::Normalize4D() noexcept {
        const auto length = CalcLength4D();
        if(length > 0.0f) {
            const auto inv_length = 1.0f / length;
            x *= inv_length;
            y *= inv_length;
            z *= inv_length;
            w *= inv_length;
            return length;
        }
        return 0.0f;
    }

    float Vector4::Normalize3D() noexcept {
        const auto length = CalcLength3D();
        if(length > 0.0f) {
            const auto inv_length = 1.0f / length;
            x *= inv_length;
            y *= inv_length;
            z *= inv_length;
            return length;
        }
        return 0.0f;
    }

    Vector4 Vector4::GetNormalize4D() const noexcept {
        const auto length = CalcLength4D();
        if(length > 0.0f) {
            const auto inv_length = 1.0f / length;
            return Vector4(x * inv_length, y * inv_length, z * inv_length, w * inv_length);
        }
        return Vector4::ZERO;
    }

    Vector4 Vector4::GetNormalize3D() const noexcept {
        const auto length = CalcLength3D();
        if(length > 0.0f) {
            const auto inv_length = 1.0f / length;
            return Vector4(x * inv_length, y * inv_length, z * inv_length, w);
        }
        return Vector4::ZERO_XYZ_ONE_W;
    }

    Vector4 Vector4::operator+(const Vector4& rhs) const noexcept {
        return Vector4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
    }

    bool Vector4::operator!=(const Vector4& rhs) const noexcept {
        return !(*this == rhs);
    }

    bool Vector4::operator==(const Vector4& rhs) const noexcept {
        return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
    }

    void swap(Vector4& a, Vector4& b) noexcept {
        std::swap(a.x, b.x);
        std::swap(a.y, b.y);
        std::swap(a.z, b.z);
        std::swap(a.w, b.w);
    }

    std::string StringUtils::to_string(const Vector4& v) noexcept {
        std::ostringstream ss;
        ss << '[' << v.x << ',' << v.y << ',' << v.z << ',' << v.w << ']';
        return ss.str();
    }

} // namespace a2de
