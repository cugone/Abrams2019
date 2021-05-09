#pragma once

#include "pch.h"

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/Vector3.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

TEST(Vector2Statics, ZeroSetsMembersToZero) {
    auto a = a2de::Vector2::ZERO;
    EXPECT_FLOAT_EQ(a.x, 0.0f);
    EXPECT_FLOAT_EQ(a.y, 0.0f);
}

TEST(Vector2Statics, ZeroEqualsDefaultCtor) {
    auto a = a2de::Vector2::ZERO;
    auto b = a2de::Vector2{};
    EXPECT_FLOAT_EQ(a.x, b.x);
    EXPECT_FLOAT_EQ(a.y, b.y);
}

TEST(Vector2Statics, ZeroEquals2ArgInitCtor) {
    auto a = a2de::Vector2::ZERO;
    auto b = a2de::Vector2{ 0.0f, 0.0f };
    EXPECT_FLOAT_EQ(a.x, b.x);
    EXPECT_FLOAT_EQ(a.y, b.y);
}

TEST(Vector2Statics, XAxisSetsXToOneYToZero) {
    auto a = a2de::Vector2::X_AXIS;
    EXPECT_FLOAT_EQ(a.x, 1.0f);
    EXPECT_FLOAT_EQ(a.y, 0.0f);
}

TEST(Vector2Statics, XAxisEquals2ArgInitCtor) {
    auto a = a2de::Vector2::X_AXIS;
    auto b = a2de::Vector2{ 1.0f, 0.0f };
    EXPECT_FLOAT_EQ(a.x, b.x);
    EXPECT_FLOAT_EQ(a.y, b.y);
}

TEST(Vector2Statics, YAxisSetsXToZeroYToOne) {
    auto a = a2de::Vector2::Y_AXIS;
    EXPECT_FLOAT_EQ(a.x, 0.0f);
    EXPECT_FLOAT_EQ(a.y, 1.0f);
}

TEST(Vector2Statics, YAxisEquals2ArgInitCtor) {
    auto a = a2de::Vector2::Y_AXIS;
    auto b = a2de::Vector2{ 0.0f, 1.0f };
    EXPECT_FLOAT_EQ(a.x, b.x);
    EXPECT_FLOAT_EQ(a.y, b.y);
}

TEST(Vector2Statics, XYAxisSetsXToOneYToOne) {
    auto a = a2de::Vector2::XY_AXIS;
    EXPECT_FLOAT_EQ(a.x, 1.0f);
    EXPECT_FLOAT_EQ(a.y, 1.0f);
}

TEST(Vector2Statics, XYAxisEquals2ArgInitCtor) {
    auto a = a2de::Vector2::XY_AXIS;
    auto b = a2de::Vector2{ 1.0f, 1.0f };
    EXPECT_FLOAT_EQ(a.x, b.x);
    EXPECT_FLOAT_EQ(a.y, b.y);
}

TEST(Vector2Statics, YXAxisSetsXToOneYToOne) {
    auto a = a2de::Vector2::YX_AXIS;
    EXPECT_FLOAT_EQ(a.x, 1.0f);
    EXPECT_FLOAT_EQ(a.y, 1.0f);
}

TEST(Vector2Statics, YXAxisEquals2ArgInitCtor) {
    auto a = a2de::Vector2::XY_AXIS;
    auto b = a2de::Vector2{ 1.0f, 1.0f };
    EXPECT_FLOAT_EQ(a.x, b.x);
    EXPECT_FLOAT_EQ(a.y, b.y);
}

TEST(Vector2Statics, OneSetsMembersToOne) {
    auto a = a2de::Vector2::ONE;
    EXPECT_FLOAT_EQ(a.x, 1.0f);
    EXPECT_FLOAT_EQ(a.y, 1.0f);
}

TEST(Vector2Statics, OneEquals2ArgInitCtor) {
    auto a = a2de::Vector2::ONE;
    auto b = a2de::Vector2{ 1.0f, 1.0f };
    EXPECT_FLOAT_EQ(a.x, b.x);
    EXPECT_FLOAT_EQ(a.y, b.y);
}

TEST(Vector2Construction, DefaultSetsMembersToZero) {
    auto a = a2de::Vector2{};
    EXPECT_FLOAT_EQ(a.x, 0.0f);
    EXPECT_FLOAT_EQ(a.y, 0.0f);
}

TEST(Vector2Construction, CopyCtor) {
    auto a = a2de::Vector2{ 1.0f, 2.0f };
    auto b = a2de::Vector2{ a };
    EXPECT_FLOAT_EQ(a.x, b.x);
    EXPECT_FLOAT_EQ(a.y, b.y);
}

TEST(Vector2Construction, MoveCtor) {
    auto a = a2de::Vector2{ 1.0f, 2.0f };
    auto b = a2de::Vector2{ std::move(a) };
    EXPECT_FLOAT_EQ(a.x, b.x);
    EXPECT_FLOAT_EQ(a.y, b.y);
}

TEST(Vector2Construction, CopyAssign) {
    auto a = a2de::Vector2{ 1.0f, 2.0f };
    auto b = a2de::Vector2{ 3.0f, 4.0f };
    b = a;
    EXPECT_FLOAT_EQ(b.x, a.x);
    EXPECT_FLOAT_EQ(b.y, a.y);
}

TEST(Vector2Construction, MoveAssign) {
    auto a = a2de::Vector2{ 1.0f, 2.0f };
    auto b = a2de::Vector2{ 3.0f, 4.0f };
    b = std::move(a);
    EXPECT_FLOAT_EQ(b.x, a.x);
    EXPECT_FLOAT_EQ(b.y, a.y);
}

TEST(Vector2Construction, FloatCtor) {
    auto a = a2de::Vector2{ 1.0f, 2.0f };
    EXPECT_FLOAT_EQ(a.x, 1.0f);
    EXPECT_FLOAT_EQ(a.y, 2.0f);
}

TEST(Vector2Construction, StringCtor) {
    auto a = a2de::Vector2{ std::string{"[3.0,4.0]"} };
    EXPECT_FLOAT_EQ(a.x, 3.0f);
    EXPECT_FLOAT_EQ(a.y, 4.0f);
}

TEST(Vector2Construction, Vector3Ctor) {
    auto a = a2de::Vector2{ a2de::Vector3{3.0f, 4.0f, 5.0f} };
    EXPECT_FLOAT_EQ(a.x, 3.0f);
    EXPECT_FLOAT_EQ(a.y, 4.0f);
}

TEST(Vector2Construction, IntVector2Ctor) {
    auto a = a2de::Vector2{ 1.3f, 2.7f };
    a = a2de::Vector2{ a2de::IntVector2{3, 4} };
    EXPECT_FLOAT_EQ(a.x, 3.0f);
    EXPECT_FLOAT_EQ(a.y, 4.0f);
}

TEST(Vector2Operators, Equality) {
    auto a = a2de::Vector2{ 1.0f, 2.0f };
    auto b = a2de::Vector2{ 1.0f, 2.0f };
    auto c = a2de::Vector2{ 1.1f, 2.0f };
    auto d = a2de::Vector2{ 1.0f, 2.1f };
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a == c);
    EXPECT_FALSE(a == d);
}

TEST(Vector2Operators, Inequality) {
    auto a = a2de::Vector2{ 1.0f, 2.0f };
    auto b = a2de::Vector2{ 1.0f, 2.0f };
    auto c = a2de::Vector2{ 1.1f, 2.0f };
    auto d = a2de::Vector2{ 1.0f, 2.1f };
    EXPECT_FALSE(a != b);
    EXPECT_TRUE(a != c);
    EXPECT_TRUE(a != d);
}

TEST(Vector2Operators, Add) {
    auto a = a2de::Vector2{ 1.0f, 2.0f };
    auto b = a2de::Vector2::ONE;
    auto c = a + b;
    EXPECT_FLOAT_EQ(a.x + b.x, c.x);
    EXPECT_FLOAT_EQ(a.y + b.y, c.y);
}

TEST(Vector2Operators, AddAssign) {
    auto a = a2de::Vector2{ 1.0f, 2.0f };
    auto b = a2de::Vector2::ONE;
    a += b;
    EXPECT_FLOAT_EQ(a.x, 2.0f);
    EXPECT_FLOAT_EQ(a.y, 3.0f);
}

TEST(Vector2Operators, Subtract) {
    auto a = a2de::Vector2{ 1.0f, 2.0f };
    auto b = a2de::Vector2::ONE;
    auto c = a - b;
    EXPECT_FLOAT_EQ(a.x - b.x, c.x);
    EXPECT_FLOAT_EQ(a.y - b.y, c.y);
}

TEST(Vector2Operators, SubtractAssign) {
    auto a = a2de::Vector2{ 1.0f, 2.0f };
    auto b = a2de::Vector2::ONE;
    a -= b;
    EXPECT_FLOAT_EQ(a.x, 0.0f);
    EXPECT_FLOAT_EQ(a.y, 1.0f);
}

TEST(Vector2Operators, Negate) {
    auto a = a2de::Vector2{ 1.0f, 2.0f };
    auto b = -a;
    EXPECT_FLOAT_EQ(-a.x, b.x);
    EXPECT_FLOAT_EQ(-a.y, b.y);
}

TEST(Vector2Operators, MultiplyRHSScalar) {
    auto a = a2de::Vector2::ONE;
    auto c = a * 2.0f;
    EXPECT_FLOAT_EQ(a.x * 2.0f, c.x);
    EXPECT_FLOAT_EQ(a.y * 2.0f, c.y);
}

TEST(Vector2Operators, MultiplyAssignScalar) {
    auto a = a2de::Vector2::ONE;
    a *= 2.0f;
    EXPECT_FLOAT_EQ(a.x, 2.0f);
    EXPECT_FLOAT_EQ(a.y, 2.0f);
}

TEST(Vector2Operators, MultiplyLHSScalar) {
    auto a = a2de::Vector2::ONE;
    auto c = 2.0f * a;
    EXPECT_FLOAT_EQ(a.x * 2.0f, c.x);
    EXPECT_FLOAT_EQ(a.y * 2.0f, c.y);
}

TEST(Vector2Operators, MultiplyLHSAndRHSScalar) {
    auto a = a2de::Vector2::ONE;
    auto c = 2.0f * a * 2.0f;
    EXPECT_FLOAT_EQ(a.x * 4.0f, c.x);
    EXPECT_FLOAT_EQ(a.y * 4.0f, c.y);
}

TEST(Vector2Operators, MultiplyVector2) {
    auto a = a2de::Vector2::ONE * 2.0f;
    auto b = a2de::Vector2{ 2.0f, 3.0f };
    auto c = a * b;
    EXPECT_FLOAT_EQ(a.x * b.x, c.x);
    EXPECT_FLOAT_EQ(a.y * b.y, c.y);
}

TEST(Vector2Operators, MultiplyAssignVector2) {
    auto a = a2de::Vector2{ 2.0f, 3.0f };
    auto b = a2de::Vector2{ 2.0f, 2.0f };
    a *= b;
    EXPECT_FLOAT_EQ(a.x, 2.0f * 2.0f);
    EXPECT_FLOAT_EQ(a.y, 3.0f * 2.0f);
}

TEST(Vector2Operators, DivideRHSScalar) {
    auto a = a2de::Vector2::ONE;
    auto c = a / 2.0f;
    EXPECT_FLOAT_EQ(a.x / 2.0f, c.x);
    EXPECT_FLOAT_EQ(a.y / 2.0f, c.y);
}

TEST(Vector2Operators, DivideAssignScalar) {
    auto a = a2de::Vector2::ONE;
    a /= 2.0f;
    EXPECT_FLOAT_EQ(a.x, 1.0f / 2.0f);
    EXPECT_FLOAT_EQ(a.y, 1.0f / 2.0f);
}

TEST(Vector2Operators, DivideVector2) {
    auto a = a2de::Vector2::ONE * 2.0f;
    auto b = a2de::Vector2{ 2.0f, 3.0f };
    auto c = a / b;
    EXPECT_FLOAT_EQ(a.x / b.x, c.x);
    EXPECT_FLOAT_EQ(a.y / b.y, c.y);
}

TEST(Vector2Operators, DivideAssignVector2) {
    auto a = a2de::Vector2{ 2.0f, 3.0f };
    auto b = a2de::Vector2{ 2.0f, 2.0f };
    a /= b;
    EXPECT_FLOAT_EQ(a.x, 2.0f / 2.0f);
    EXPECT_FLOAT_EQ(a.y, 3.0f / 2.0f);
}

TEST(Vector2Operators, StreamIn) {
    auto a = a2de::Vector2{};
    {
        std::istringstream ss;
        ss.str("[1.0,2.0]");
        ss.clear();
        ss.seekg(0);
        ss >> a;
        EXPECT_FLOAT_EQ(a.x, 1.0f);
        EXPECT_FLOAT_EQ(a.y, 2.0f);
    }
    {
        std::istringstream ss;
        ss.str("[4,3]");
        ss.clear();
        ss.seekg(0);
        ss >> a;
        EXPECT_FLOAT_EQ(a.x, 4.0f);
        EXPECT_FLOAT_EQ(a.y, 3.0f);
    }
}

TEST(Vector2Operators, StreamOut) {
    auto a = a2de::Vector2{ 1.0f, 2.0f };
    std::ostringstream ss;
    ss << a;
    EXPECT_FLOAT_EQ(a.x, 1.0f);
    EXPECT_FLOAT_EQ(a.y, 2.0f);
    EXPECT_STREQ(ss.str().c_str(), "[1,2]");
    ss.str("");
    a = a2de::Vector2{ 1.1f, 2.2f };
    ss << a;
    EXPECT_FLOAT_EQ(a.x, 1.1f);
    EXPECT_FLOAT_EQ(a.y, 2.2f);
    EXPECT_STREQ(ss.str().c_str(), "[1.1,2.2]");
}


TEST(Vector2MemberFunctions, GetXY) {
    auto a = a2de::Vector2{ 2.0f, 3.0f };
    auto x = 0.0f;
    auto y = 0.0f;
    a.GetXY(x, y);
    EXPECT_FLOAT_EQ(a.x, x);
    EXPECT_FLOAT_EQ(a.y, y);
}

TEST(Vector2MemberFunctions, GetAsFloatArray) {
    auto a = a2de::Vector2{ 2.0f, 3.0f };
    auto p_ar = a.GetAsFloatArray();
    auto p_ax = &a.x;
    auto p_ay = &a.y;
    auto arx = p_ar[0];
    auto ary = p_ar[1];
    auto p_arx = &p_ar[0];
    auto p_ary = &p_ar[1];
    EXPECT_FLOAT_EQ(a.x, arx);
    EXPECT_FLOAT_EQ(a.y, ary);
    EXPECT_TRUE(p_ax == p_ar);
    EXPECT_TRUE(p_ax == p_arx);
    EXPECT_TRUE(p_ay == p_ary);
}

TEST(Vector2MemberFunctions, CalcHeadingRadians) {
    auto a = a2de::Vector2{0.8f, 0.6f};
    float expected = std::atan2(0.6f, 0.8f);
    EXPECT_FLOAT_EQ(a.CalcHeadingRadians(), expected);
    a = a2de::Vector2{-0.8f, 0.6f};
    expected = std::atan2(0.6f, -0.8f);
    EXPECT_FLOAT_EQ(a.CalcHeadingRadians(), expected);
    a = a2de::Vector2{0.8f, -0.6f};
    expected = std::atan2(-0.6f, 0.8f);
    EXPECT_FLOAT_EQ(a.CalcHeadingRadians(), expected);
    a = a2de::Vector2{-0.8f,-0.6f };
    expected = std::atan2(-0.6f,-0.8f);
    EXPECT_FLOAT_EQ(a.CalcHeadingRadians(), expected);
    auto b = a2de::Vector2{1.0f, 0.0f};
    expected = std::atan2(0.0f, 1.0f);
    EXPECT_FLOAT_EQ(b.CalcHeadingRadians(), expected);
    b = a2de::Vector2{-1.0f, 0.0f};
    expected = std::atan2(0.0f, -1.0f);
    EXPECT_FLOAT_EQ(b.CalcHeadingRadians(), expected);
    b = a2de::Vector2{1.0f, -0.0f};
    expected = std::atan2(-0.0f, 1.0f);
    EXPECT_FLOAT_EQ(b.CalcHeadingRadians(), expected);
    b = a2de::Vector2{-1.0f, -0.0f};
    expected = std::atan2(-0.0f, -1.0f);
    EXPECT_FLOAT_EQ(b.CalcHeadingRadians(), expected);
    auto c = a2de::Vector2{0.0f, 1.0f};
    expected = std::atan2(1.0f, 0.0f);
    EXPECT_FLOAT_EQ(c.CalcHeadingRadians(), expected);
    c = a2de::Vector2{0.0f, -1.0f};
    expected = std::atan2(-1.0f, 0.0f);
    EXPECT_FLOAT_EQ(c.CalcHeadingRadians(), expected);
    c = a2de::Vector2{-0.0f, 1.0f};
    expected = std::atan2(1.0f, -0.0f);
    EXPECT_FLOAT_EQ(c.CalcHeadingRadians(), expected);
    c = a2de::Vector2{-0.0f, -1.0f};
    expected = std::atan2(-1.0f, -0.0f);
    EXPECT_FLOAT_EQ(c.CalcHeadingRadians(), expected);
    auto d = a2de::Vector2{1.0f, 1.0f};
    expected = std::atan2(1.0f, 1.0f);
    EXPECT_FLOAT_EQ(d.CalcHeadingRadians(), expected);
    d = a2de::Vector2{-1.0f, 1.0f};
    expected = std::atan2(1.0f, -1.0f);
    EXPECT_FLOAT_EQ(d.CalcHeadingRadians(), expected);
    d = a2de::Vector2{1.0f, -1.0f};
    expected = std::atan2(-1.0f, 1.0f);
    EXPECT_FLOAT_EQ(d.CalcHeadingRadians(), expected);
    d = a2de::Vector2{-1.0f, -1.0f};
    expected = std::atan2(-1.0f, -1.0f);
    EXPECT_FLOAT_EQ(d.CalcHeadingRadians(), expected);
}

TEST(Vector2MemberFunctions, CalcHeadingDegrees) {
    auto a = a2de::Vector2::X_AXIS;
    auto expected = 0.0f;
    EXPECT_FLOAT_EQ(a.CalcHeadingDegrees(), expected);
    a = a2de::Vector2{ 1.0f, 1.0f };
    expected = 45.0f;
    EXPECT_FLOAT_EQ(a.CalcHeadingDegrees(), expected);
    a = a2de::Vector2::Y_AXIS;
    expected = 90.0f;
    EXPECT_FLOAT_EQ(a.CalcHeadingDegrees(), expected);
    a = a2de::Vector2{-1.0f, 1.0f};
    expected = 135.0f;
    EXPECT_FLOAT_EQ(a.CalcHeadingDegrees(), expected);
    a = -a2de::Vector2::X_AXIS;
    expected = -180.0f;
    EXPECT_FLOAT_EQ(a.CalcHeadingDegrees(), expected);
    a = a2de::Vector2{ -1.0f, -1.0f };
    expected = -135.0f;
    EXPECT_FLOAT_EQ(a.CalcHeadingDegrees(), expected);
    a = -a2de::Vector2::Y_AXIS;
    expected = -90.0f;
    EXPECT_FLOAT_EQ(a.CalcHeadingDegrees(), expected);
}

//float CalcHeadingDegrees() const;
//float CalcLength() const;
//float CalcLengthSquared() const;
//
//
//void SetHeadingDegrees(float headingDegrees);
//void SetHeadingRadians(float headingRadians);
//
//void SetUnitLengthAndHeadingDegrees(float headingDegrees);
//void SetUnitLengthAndHeadingRadians(float headingRadians);
//float SetLength(float length);
//void SetLengthAndHeadingDegrees(float headingDegrees, float length);
//void SetLengthAndHeadingRadians(float headingRadians, float length);
//
//float Normalize();
//Vector2 GetNormalize() const;
//
//void Rotate90Degrees();
//void RotateNegative90Degrees();
//void RotateRadians(float radians);
//
//void SetXY(float newX, float newY);
//
//friend void swap(Vector2& a, Vector2& b) noexcept;
