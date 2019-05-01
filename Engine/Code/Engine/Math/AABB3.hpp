#pragma once

#include "Engine/Math/Vector3.hpp"

class AABB3 {
public:

    Vector3 mins = Vector3::ZERO;
    Vector3 maxs = Vector3::ZERO;

    static const AABB3 ZERO_TO_ONE;
    static const AABB3 NEG_ONE_TO_ONE;

    AABB3() = default;
    AABB3(const AABB3& rhs) = default;
    AABB3(AABB3&& rhs) = default;
    AABB3& operator=(const AABB3& rhs) = default;
    AABB3& operator=(AABB3&& rhs) = default;
    ~AABB3() = default;
    AABB3(float initialX, float initialY, float initialZ);
    AABB3(float minX, float minY, float maxX, float maxY, float minZ, float maxZ);
    AABB3(const Vector3& mins, const Vector3& maxs);
    AABB3(const Vector3& center, float radiusX, float radiusY, float radiusZ);

    void StretchToIncludePoint(const Vector3& point);
    void AddPaddingToSides(float paddingX, float paddingY, float paddingZ);
    void AddPaddingToSidesClamped(float paddingX, float paddingY, float paddingZ);
    void Translate(const Vector3& translation);

    const Vector3 CalcDimensions() const;
    const Vector3 CalcCenter() const;

    AABB3 operator+(const Vector3& translation) const;
    AABB3 operator-(const Vector3& antiTranslation) const;
    AABB3& operator+=(const Vector3& translation);
    AABB3& operator-=(const Vector3& antiTranslation);

protected:
private:
};