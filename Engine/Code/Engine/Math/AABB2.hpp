#pragma once

#include "Engine/Math/Vector2.hpp"

class AABB2 {
public:

    Vector2 mins = Vector2::ZERO;
    Vector2 maxs = Vector2::ZERO;

    static const AABB2 ZERO_TO_ONE;
    static const AABB2 NEG_ONE_TO_ONE;

    AABB2() = default;
    AABB2(const AABB2& rhs) = default;
    AABB2(AABB2&& rhs) = default;
    AABB2& operator=(const AABB2& rhs) = default;
    AABB2& operator=(AABB2&& rhs) = default;
    ~AABB2() = default;
    AABB2(float initialX, float initialY);
    AABB2(float minX, float minY, float maxX, float maxY);
    AABB2(const Vector2& mins, const Vector2& maxs);
    AABB2(const Vector2& center, float radiusX, float radiusY);

    void StretchToIncludePoint(const Vector2& point);
    void AddPaddingToSides(float paddingX, float paddingY);
    void AddPaddingToSidesClamped(float paddingX, float paddingY);
    void Translate(const Vector2& translation);

    Vector2 CalcDimensions() const;
    Vector2 CalcCenter() const;

    AABB2 operator+(const Vector2& translation) const;
    AABB2 operator-(const Vector2& antiTranslation) const;
    AABB2& operator+=(const Vector2& translation);
    AABB2& operator-=(const Vector2& antiTranslation);

protected:
private:
};