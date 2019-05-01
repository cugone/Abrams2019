#pragma once

#include "Engine/Math/Vector2.hpp"

class Disc2 {
public:

    static const Disc2 UNIT_CIRCLE;

    Disc2() = default;
    Disc2(const Disc2& rhs) = default;
    Disc2& operator=(const Disc2& rhs) = default;
    Disc2& operator=(Disc2&& rhs) = default;
    ~Disc2() = default;
    explicit Disc2(float initialX, float initialY, float initialRadius);
    explicit Disc2(const Vector2& initialCenter, float initialRadius);

    void StretchToIncludePoint(const Vector2& point);
    void AddPadding(float paddingRadius);
    void Translate(const Vector2& translation);

    Disc2 operator-(const Vector2& antiTranslation);
    Disc2 operator+(const Vector2& translation);
    Disc2& operator+=(const Vector2& translation);
    Disc2& operator-=(const Vector2& antiTranslation);

    Vector2 center = Vector2::ZERO;
    float radius = 0.0f;

protected:
private:
};