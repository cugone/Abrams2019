#include "Engine/Math/AABB3.hpp"

#include <algorithm>

const AABB3 AABB3::ZERO_TO_ONE(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
const AABB3 AABB3::NEG_ONE_TO_ONE(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f);

AABB3::AABB3(float initialX, float initialY, float initialZ)
    : mins(initialX, initialY, initialZ)
    , maxs(initialX,initialY, initialZ)
{
    /* DO NOTHING */
}

AABB3::AABB3(float minX, float minY, float maxX, float maxY, float minZ, float maxZ)
    : mins(minX, minY, minZ)
    , maxs(maxX, maxY, maxZ)
{
    /* DO NOTHING */
}

AABB3::AABB3(const Vector3& mins, const Vector3& maxs)
    : mins(mins)
    , maxs(maxs)
{
    /* DO NOTHING */
}

AABB3::AABB3(const Vector3& center, float radiusX, float radiusY, float radiusZ)
    : mins(center.x - radiusX, center.y - radiusY, center.z - radiusZ)
    , maxs(center.x + radiusX, center.y + radiusY, center.z + radiusZ)
{
    /* DO NOTHING */
}

void AABB3::StretchToIncludePoint(const Vector3& point) {
    if(point.x < mins.x) {
        mins.x = point.x;
    }
    if(point.y < mins.y) {
        mins.y = point.y;
    }
    if(point.z < mins.z) {
        mins.z = point.z;
    }
    if(maxs.x < point.x) {
        maxs.x = point.x;
    }
    if(maxs.y < point.y) {
        maxs.y = point.y;
    }
    if(maxs.z < point.z) {
        maxs.z = point.z;
    }
}

void AABB3::AddPaddingToSides(float paddingX, float paddingY, float paddingZ) {
    mins.x -= paddingX;
    mins.y -= paddingY;
    mins.z -= paddingZ;

    maxs.x += paddingX;
    maxs.y += paddingY;
    maxs.z += paddingZ;
}

void AABB3::AddPaddingToSidesClamped(float paddingX, float paddingY, float paddingZ) {
    auto width = maxs.x - mins.x;
    auto height = maxs.y - mins.y;
    auto depth = maxs.z - mins.z;

    auto half_width = width * 0.5f;
    auto half_height = height * 0.5f;
    auto half_depth = depth * 0.5f;

    paddingX = (std::max)(-half_width, paddingX);
    paddingY = (std::max)(-half_height, paddingY);
    paddingZ = (std::max)(-half_depth, paddingZ);

    mins.x -= paddingX;
    mins.y -= paddingY;
    mins.z -= paddingZ;

    maxs.x += paddingX;
    maxs.y += paddingY;
    maxs.z += paddingZ;
}

void AABB3::Translate(const Vector3& translation) {
    mins += translation;
    maxs += translation;
}

const Vector3 AABB3::CalcDimensions() const {
    return Vector3(maxs.x - mins.x, maxs.y - mins.y, maxs.z - mins.z);
}

const Vector3 AABB3::CalcCenter() const {
    return Vector3(mins.x + (maxs.x - mins.x) * 0.5f, mins.y + (maxs.y - mins.y) * 0.5f, mins.z + (maxs.z - mins.z) * 0.5f);
}

AABB3 AABB3::operator+(const Vector3& translation) const {
    return AABB3(mins.x + translation.x,
                 mins.y + translation.y,
                 mins.z + translation.z,
                 maxs.x + translation.x,
                 maxs.y + translation.y,
                 maxs.z + translation.z);
}
AABB3 AABB3::operator-(const Vector3& antiTranslation) const {
    return AABB3(mins.x - antiTranslation.x,
                 mins.y - antiTranslation.y,
                 mins.z - antiTranslation.z,
                 maxs.x - antiTranslation.x,
                 maxs.y - antiTranslation.y,
                 maxs.z - antiTranslation.z);
}

AABB3& AABB3::operator-=(const Vector3& antiTranslation) {
    mins -= antiTranslation;
    maxs -= antiTranslation;
    return *this;
}

AABB3& AABB3::operator+=(const Vector3& translation) {
    mins += translation;
    maxs += translation;
    return *this;
}

