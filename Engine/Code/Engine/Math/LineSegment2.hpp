#pragma once

#include "Engine/Math/Vector2.hpp"

namespace a2de {

    class LineSegment2 {
    public:
        Vector2 start = Vector2::ZERO;
        Vector2 end = Vector2::ZERO;

        static const LineSegment2 UNIT_HORIZONTAL;
        static const LineSegment2 UNIT_VERTICAL;
        static const LineSegment2 UNIT_CENTERED_HORIZONTAL;
        static const LineSegment2 UNIT_CENTERED_VERTICAL;

        LineSegment2() = default;
        LineSegment2(const LineSegment2& rhs) = default;
        LineSegment2(LineSegment2&& rhs) = default;
        ~LineSegment2() = default;
        LineSegment2& operator=(LineSegment2&& rhs) = default;
        LineSegment2& operator=(const LineSegment2& rhs) = default;

        explicit LineSegment2(float startX, float startY, float endX, float endY) noexcept;
        explicit LineSegment2(const Vector2& startPosition, const Vector2& endPosition) noexcept;
        explicit LineSegment2(const Vector2& startPosition, const Vector2& direction, float length) noexcept;
        explicit LineSegment2(const Vector2& startPosition, float angleDegrees, float length) noexcept;

        void SetLengthFromStart(float length) noexcept;
        void SetLengthFromCenter(float length) noexcept;
        void SetLengthFromEnd(float length) noexcept;

        [[nodiscard]] Vector2 CalcCenter() const noexcept;

        [[nodiscard]] float CalcLength() const noexcept;
        [[nodiscard]] float CalcLengthSquared() const noexcept;

        void SetDirectionFromStart(float angleDegrees) noexcept;
        void SetDirectionFromCenter(float angleDegrees) noexcept;
        void SetDirectionFromEnd(float angleDegrees) noexcept;

        void SetStartEndPositions(const Vector2& startPosition, const Vector2& endPosition) noexcept;

        void Translate(const Vector2& translation) noexcept;

        void Rotate(float angleDegrees) noexcept;
        void RotateStartPosition(float angleDegrees) noexcept;
        void RotateEndPosition(float angleDegrees) noexcept;
        void Rotate90Degrees() noexcept;
        void RotateNegative90Degrees() noexcept;
        void Rotate180Degrees() noexcept;

        [[nodiscard]] Vector2 CalcDisplacement() const noexcept;
        [[nodiscard]] Vector2 CalcDirection() const noexcept;
        [[nodiscard]] Vector2 CalcPositiveNormal() const noexcept;
        [[nodiscard]] Vector2 CalcNegativeNormal() const noexcept;

        [[nodiscard]] LineSegment2 operator+(const Vector2& translation) const noexcept;
        [[nodiscard]] LineSegment2 operator-(const Vector2& antiTranslation) const noexcept;
        LineSegment2& operator+=(const Vector2& translation) noexcept;
        LineSegment2& operator-=(const Vector2& antiTranslation) noexcept;

    protected:
    private:
        void SetAngle(float angleDegrees) noexcept;

        friend class Capsule2;
    };

} // namespace a2de
