#pragma once

#include "Engine/Math/Vector3.hpp"

class Sphere3 {
public:

    Vector3 center = Vector3::ZERO;
    float radius = 0.0f;

    static const Sphere3 UNIT_SPHERE;

    Sphere3() = default;
    Sphere3(const Sphere3& rhs) = default;
    Sphere3(Sphere3&& rhs) = default;
    Sphere3& operator=(const Sphere3& rhs) = default;
    Sphere3& operator=(Sphere3&& rhs) = default;
    ~Sphere3() = default;

    explicit Sphere3(float initialX, float initialY, float initialZ, float initialRadius);
    explicit Sphere3(const Vector3& initialCenter, float initialRadius);

    void StretchToIncludePoint(const Vector3& point);
    void AddPadding(float paddingRadius);
    void Translate(const Vector3& translation);
    
    Sphere3 operator+(const Vector3& translation) const;
    Sphere3 operator-(const Vector3& antiTranslation) const;

    Sphere3& operator+=(const Vector3& translation);
    Sphere3& operator-=(const Vector3& antiTranslation);

protected:
private:
};