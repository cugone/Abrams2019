#include "Engine/Math/AABB2.hpp"

#include <algorithm>

const AABB2 AABB2::ZERO_TO_ONE(0.0f, 0.0f, 1.0f, 1.0f);
const AABB2 AABB2::NEG_ONE_TO_ONE(-1.0f, -1.0f, 1.0f, 1.0f);

AABB2::AABB2(float initialX, float initialY)
    : mins(initialX, initialY)
    , maxs(initialX, initialY)
{
    /* DO NOTHING */
}

AABB2::AABB2(float minX, float minY, float maxX, float maxY)
    : mins(minX, minY)
    , maxs(maxX, maxY)
{
    /* DO NOTHING */
}

AABB2::AABB2(const Vector2& mins, const Vector2& maxs)
    : mins(mins)
    , maxs(maxs)
{
    /* DO NOTHING */
}

AABB2::AABB2(const Vector2& center, float radiusX, float radiusY)
    : mins(center.x - radiusX, center.y - radiusY)
    , maxs(center.x + radiusX, center.y + radiusY)
{
    /* DO NOTHING */
}

void AABB2::StretchToIncludePoint(const Vector2& point) {
    if(point.x < mins.x) {
        mins.x = point.x;
    }
    if(point.y < mins.y) {
        mins.y = point.y;
    }
    if(maxs.x < point.x) {
        maxs.x = point.x;
    }
    if(maxs.y < point.y) {
        maxs.y = point.y;
    }
}

void AABB2::AddPaddingToSides(float paddingX, float paddingY) {
    mins.x -= paddingX;
    mins.y -= paddingY;

    maxs.x += paddingX;
    maxs.y += paddingY;
}

void AABB2::AddPaddingToSidesClamped(float paddingX, float paddingY) {
    auto width = maxs.x - mins.x;
    auto height = maxs.y - mins.y;
    auto half_width = width * 0.5f;
    auto half_height = height * 0.5f;

    paddingX = (std::max)(-half_width, paddingX);
    paddingY = (std::max)(-half_height, paddingY);

    mins.x -= paddingX;
    mins.y -= paddingY;

    maxs.x += paddingX;
    maxs.y += paddingY;
}

void AABB2::Translate(const Vector2& translation) {
    mins += translation;
    maxs += translation;
}

Vector2 AABB2::CalcDimensions() const {
    return Vector2(maxs.x - mins.x, maxs.y - mins.y);
}

Vector2 AABB2::CalcCenter() const {
    return Vector2(mins.x + (maxs.x - mins.x) * 0.5f, mins.y + (maxs.y - mins.y) * 0.5f);
}

AABB2 AABB2::operator+(const Vector2& translation) const {
    return AABB2(mins.x + translation.x, mins.y + translation.y, maxs.x + translation.x, maxs.y + translation.y);
}

AABB2 AABB2::operator-(const Vector2& antiTranslation) const {
    return AABB2(mins.x - antiTranslation.x, mins.y - antiTranslation.y, maxs.x - antiTranslation.x, maxs.y - antiTranslation.y);
}

AABB2& AABB2::operator-=(const Vector2& antiTranslation) {
    mins -= antiTranslation;
    maxs -= antiTranslation;
    return *this;
}

AABB2& AABB2::operator+=(const Vector2& translation) {
    mins += translation;
    maxs += translation;
    return *this;
}
