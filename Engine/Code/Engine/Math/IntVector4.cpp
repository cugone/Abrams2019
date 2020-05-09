#include "Engine/Math/IntVector4.hpp"

#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/IntVector3.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"

#include <cmath>
#include <sstream>

const IntVector4 IntVector4::ZERO(0, 0, 0, 0);
const IntVector4 IntVector4::ONE(1, 1, 1, 1);
const IntVector4 IntVector4::X_AXIS(1, 0, 0, 0);
const IntVector4 IntVector4::Y_AXIS(0, 1, 0, 0);
const IntVector4 IntVector4::Z_AXIS(0, 0, 1, 0);
const IntVector4 IntVector4::W_AXIS(0, 0, 0, 1);
const IntVector4 IntVector4::XY_AXIS(1, 1, 0, 0);
const IntVector4 IntVector4::XZ_AXIS(1, 0, 1, 0);
const IntVector4 IntVector4::XW_AXIS(1, 0, 0, 1);
const IntVector4 IntVector4::YX_AXIS(1, 1, 0, 0);
const IntVector4 IntVector4::YZ_AXIS(0, 1, 1, 0);
const IntVector4 IntVector4::YW_AXIS(0, 1, 0, 1);
const IntVector4 IntVector4::ZX_AXIS(1, 0, 1, 0);
const IntVector4 IntVector4::ZY_AXIS(0, 1, 1, 0);
const IntVector4 IntVector4::ZW_AXIS(0, 0, 1, 1);
const IntVector4 IntVector4::WX_AXIS(1, 0, 0, 1);
const IntVector4 IntVector4::WY_AXIS(0, 1, 0, 1);
const IntVector4 IntVector4::WZ_AXIS(0, 0, 1, 1);
const IntVector4 IntVector4::XYZ_AXIS(1, 1, 1, 0);
const IntVector4 IntVector4::XYW_AXIS(1, 1, 0, 1);
const IntVector4 IntVector4::YXZ_AXIS(1, 1, 1, 0);
const IntVector4 IntVector4::YZW_AXIS(0, 1, 1, 1);
const IntVector4 IntVector4::WXY_AXIS(1, 1, 0, 1);
const IntVector4 IntVector4::WXZ_AXIS(1, 0, 1, 1);
const IntVector4 IntVector4::WYZ_AXIS(0, 1, 1, 1);
const IntVector4 IntVector4::XYZW_AXIS(1, 1, 1, 1);

IntVector4::IntVector4(int initialX, int initialY, int initialZ, int initialW) noexcept
: x(initialX)
, y(initialY)
, z(initialZ)
, w(initialW) {
    /* DO NOTHING */
}

IntVector4::IntVector4(const IntVector2& iv2, int initialZ, int initialW) noexcept
: x(iv2.x)
, y(iv2.y)
, z(initialZ)
, w(initialW) {
    /* DO NOTHING */
}

IntVector4::IntVector4(const Vector2& v2, int initialZ, int initialW) noexcept
: x(static_cast<int>(std::floor(v2.x)))
, y(static_cast<int>(std::floor(v2.y)))
, z(initialZ)
, w(initialW) {
    /* DO NOTHING */
}

IntVector4::IntVector4(const Vector2& xy, const Vector2& zw) noexcept
: x(static_cast<int>(std::floor(xy.x)))
, y(static_cast<int>(std::floor(xy.y)))
, z(static_cast<int>(std::floor(zw.x)))
, w(static_cast<int>(std::floor(zw.y))) {
    /* DO NOTHING */
}

IntVector4::IntVector4(const IntVector2& xy, const IntVector2& zw) noexcept
: x(xy.x)
, y(xy.y)
, z(zw.x)
, w(zw.y) {
    /* DO NOTHING */
}

IntVector4::IntVector4(const IntVector3& iv3, int initialW) noexcept
: x(iv3.x)
, y(iv3.y)
, z(iv3.z)
, w(initialW) {
    /* DO NOTHING */
}

IntVector4::IntVector4(const Vector3& v3, int initialW) noexcept
: x(static_cast<int>(std::floor(v3.x)))
, y(static_cast<int>(std::floor(v3.y)))
, z(static_cast<int>(std::floor(v3.z)))
, w(initialW) {
    /* DO NOTHING */
}

IntVector4::IntVector4(const Vector4& rhs) noexcept
: x(static_cast<int>(std::floor(rhs.x)))
, y(static_cast<int>(std::floor(rhs.y)))
, z(static_cast<int>(std::floor(rhs.z)))
, w(static_cast<int>(std::floor(rhs.w))) {
    /* DO NOTHING */
}

IntVector4::IntVector4(const std::string& value) noexcept
: x(0)
, y(0)
, z(0)
, w(0) {
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
                case 3: w = std::stoi(values[i]); break;
                default: break;
                }
            }
        }
    }
}

IntVector2 IntVector4::GetXY() const noexcept {
    return IntVector2{x, y};
}

IntVector2 IntVector4::GetZW() const noexcept {
    return IntVector2{z, w};
}

void IntVector4::SetXYZW(int newX, int newY, int newZ, int newW) noexcept {
    x = newX;
    y = newY;
    z = newZ;
    w = newW;
}

std::tuple<int, int, int, int> IntVector4::GetXYZW() const noexcept {
    return std::make_tuple(x, y, z, w);
}

bool IntVector4::operator!=(const IntVector4& rhs) noexcept {
    return !(*this == rhs);
}

bool IntVector4::operator==(const IntVector4& rhs) noexcept {
    return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
}
