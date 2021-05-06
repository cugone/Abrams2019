#pragma once

#include <string>

namespace a2de {

    class Vector3;
    class IntVector2;

    class Vector2 {
    public:
        static const Vector2 ONE;
        static const Vector2 ZERO;
        static const Vector2 X_AXIS;
        static const Vector2 Y_AXIS;
        static const Vector2 XY_AXIS;
        static const Vector2 YX_AXIS;

        [[nodiscard]] static Vector2 CreateFromPolarCoordinatesDegrees(float length, float directionDegrees);
        [[nodiscard]] static Vector2 CreateFromPolarCoordinatesRadians(float length, float directionRadians);

        Vector2() noexcept = default;
        explicit Vector2(const std::string& value) noexcept;
        explicit Vector2(float initialX, float initialY) noexcept;
        explicit Vector2(const Vector3& rhs) noexcept;
        explicit Vector2(const IntVector2& intvec2) noexcept;

        [[nodiscard]] Vector2 operator+(const Vector2& rhs) const noexcept;
        Vector2& operator+=(const Vector2& rhs) noexcept;

        [[nodiscard]] Vector2 operator-() const noexcept;
        [[nodiscard]] Vector2 operator-(const Vector2& rhs) const noexcept;
        Vector2& operator-=(const Vector2& rhs) noexcept;

        friend Vector2 operator*(float lhs, const Vector2& rhs) noexcept;
        [[nodiscard]] Vector2 operator*(float scalar) const noexcept;
        Vector2& operator*=(float scalar) noexcept;
        [[nodiscard]] Vector2 operator*(const Vector2& rhs) const noexcept;
        Vector2& operator*=(const Vector2& rhs) noexcept;

        [[nodiscard]] Vector2 operator/(float scalar) const noexcept;
        Vector2 operator/=(float scalar) noexcept;
        [[nodiscard]] Vector2 operator/(const Vector2& rhs) const noexcept;
        Vector2 operator/=(const Vector2& rhs) noexcept;

        [[nodiscard]] bool operator==(const Vector2& rhs) const noexcept;
        [[nodiscard]] bool operator!=(const Vector2& rhs) const noexcept;

        friend std::ostream& operator<<(std::ostream& out_stream, const Vector2& v) noexcept;
        friend std::istream& operator>>(std::istream& in_stream, Vector2& v) noexcept;

        //TODO: Vector2::GetXY should return a tuple
        void GetXY(float& outX, float& outY) const noexcept;
        [[nodiscard]] float* GetAsFloatArray() noexcept;

        [[nodiscard]] float CalcHeadingRadians() const noexcept;
        [[nodiscard]] float CalcHeadingDegrees() const noexcept;
        [[nodiscard]] float CalcLength() const noexcept;
        [[nodiscard]] float CalcLengthSquared() const noexcept;

        void SetHeadingDegrees(float headingDegrees) noexcept;
        void SetHeadingRadians(float headingRadians) noexcept;

        void SetUnitLengthAndHeadingDegrees(float headingDegrees) noexcept;
        void SetUnitLengthAndHeadingRadians(float headingRadians) noexcept;
        float SetLength(float length) noexcept;
        void SetLengthAndHeadingDegrees(float headingDegrees, float length) noexcept;
        void SetLengthAndHeadingRadians(float headingRadians, float length) noexcept;

        float Normalize() noexcept;
        [[nodiscard]] Vector2 GetNormalize() const noexcept;

        [[nodiscard]] Vector2 GetLeftHandNormal() const noexcept;
        [[nodiscard]] Vector2 GetRightHandNormal() const noexcept;
        void Rotate90Degrees() noexcept;
        void RotateNegative90Degrees() noexcept;
        void RotateRadians(float radians) noexcept;
        void RotateDegrees(float degrees) noexcept;
        void SetXY(float newX, float newY) noexcept;

        float x = 0.0f;
        float y = 0.0f;

        friend void swap(Vector2& a, Vector2& b) noexcept;

    protected:
    private:
    };

    namespace StringUtils {
        [[nodiscard]] std::string to_string(const Vector2& v) noexcept;
    }

} // namespace a2de
