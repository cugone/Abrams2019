#pragma once

#include "Engine/Math/Vector2.hpp"

class OBB2 {
public:

    Vector2 half_extents{};
    Vector2 position{};
    float orientationDegrees = 0.0f;

    OBB2() = default;
    OBB2(const OBB2& other) = default;
    OBB2(OBB2&& other) = default;
    OBB2& operator=(const OBB2& other) = default;
    OBB2& operator=(OBB2&& other) = default;
    OBB2(const Vector2& initialPosition, float initialOrientationDegrees);
    OBB2(float initialX, float initialY, float initialOrientationDegrees);
    OBB2(const Vector2& center, const Vector2& halfExtents, float orientationDegrees);
    OBB2(const Vector2& center, float halfExtentX, float halfExtentY, float orientationDegrees);
    ~OBB2() = default;

    void SetOrientationDegrees(float newOrientationDegrees);
    void SetOrientation(float newOrientationRadians);
    void RotateDegrees(float rotationDegrees);
    void Rotate(float rotationRadians);
    void StretchToIncludePoint(const Vector2& point);
    void AddPaddingToSides(float paddingX, float paddingY);
    void AddPaddingToSides(const Vector2& padding);
    void AddPaddingToSidesClamped(float paddingX, float paddingY);
    void AddPaddingToSidesClamped(const Vector2& padding);
    void Translate(const Vector2& translation);

    Vector2 GetRight() const;
    Vector2 GetUp() const;
    Vector2 GetLeft() const;
    Vector2 GetDown() const;

    Vector2 CalcDimensions() const;
    Vector2 CalcCenter() const;

    OBB2 operator+(const Vector2& translation) const;
    OBB2 operator-(const Vector2& antiTranslation) const;
    OBB2& operator+=(const Vector2& translation);
    OBB2& operator-=(const Vector2& antiTranslation);

protected:
private:
};