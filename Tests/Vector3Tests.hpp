#pragma once

#include "pch.h"

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/IntVector3.hpp"
#include "Engine/Math/Vector3.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

TEST(Vector3Statics, ZeroSetsMembersToZero) {
    auto a = a2de::Vector3::ZERO;
    EXPECT_FLOAT_EQ(a.x, 0.0f);
    EXPECT_FLOAT_EQ(a.y, 0.0f);
    EXPECT_FLOAT_EQ(a.z, 0.0f);
}

TEST(Vector3Statics, ZeroEqualsDefaultCtor) {
    auto a = a2de::Vector3::ZERO;
    auto b = a2de::Vector3{};
    EXPECT_FLOAT_EQ(a.x, b.x);
    EXPECT_FLOAT_EQ(a.y, b.y);
    EXPECT_FLOAT_EQ(a.z, b.z);
}

TEST(Vector3Statics, ZeroEquals3ArgInitCtor) {
    auto a = a2de::Vector3::ZERO;
    auto b = a2de::Vector3{ 0.0f, 0.0f, 0.0f };
    EXPECT_FLOAT_EQ(a.x, b.x);
    EXPECT_FLOAT_EQ(a.y, b.y);
    EXPECT_FLOAT_EQ(a.z, b.z);
}

TEST(Vector3Statics, XAxisSetsXToOneYToZeroZToZero) {
    auto a = a2de::Vector3::X_AXIS;
    EXPECT_FLOAT_EQ(a.x, 1.0f);
    EXPECT_FLOAT_EQ(a.y, 0.0f);
    EXPECT_FLOAT_EQ(a.z, 0.0f);
}

TEST(Vector3Statics, XAxisEquals3ArgInitCtor) {
    auto a = a2de::Vector3::X_AXIS;
    auto b = a2de::Vector3{ 1.0f, 0.0f, 0.0f };
    EXPECT_FLOAT_EQ(a.x, b.x);
    EXPECT_FLOAT_EQ(a.y, b.y);
    EXPECT_FLOAT_EQ(a.z, b.z);
}

TEST(Vector3Statics, XYAxisSetsXToOneYToOneZToZero) {
    auto a = a2de::Vector3::XY_AXIS;
    EXPECT_FLOAT_EQ(a.x, 1.0f);
    EXPECT_FLOAT_EQ(a.y, 1.0f);
    EXPECT_FLOAT_EQ(a.z, 0.0f);
}

TEST(Vector3Statics, XYAxisEquals3ArgInitCtor) {
    auto a = a2de::Vector3::XY_AXIS;
    auto b = a2de::Vector3{ 1.0f, 1.0f, 0.0f };
    EXPECT_FLOAT_EQ(a.x, b.x);
    EXPECT_FLOAT_EQ(a.y, b.y);
    EXPECT_FLOAT_EQ(a.z, b.z);
}

TEST(Vector3Statics, XZAxisSetsXToOneYToZeroZToOne) {
    auto a = a2de::Vector3::XZ_AXIS;
    EXPECT_FLOAT_EQ(a.x, 1.0f);
    EXPECT_FLOAT_EQ(a.y, 0.0f);
    EXPECT_FLOAT_EQ(a.z, 1.0f);
}

TEST(Vector3Statics, XZAxisEquals3ArgInitCtor) {
    auto a = a2de::Vector3::XZ_AXIS;
    auto b = a2de::Vector3{ 1.0f, 0.0f, 1.0f };
    EXPECT_FLOAT_EQ(a.x, b.x);
    EXPECT_FLOAT_EQ(a.y, b.y);
    EXPECT_FLOAT_EQ(a.z, b.z);
}

TEST(Vector3Statics, YAxisSetsXToZeroYToOneZToZero) {
    auto a = a2de::Vector3::Y_AXIS;
    EXPECT_FLOAT_EQ(a.x, 0.0f);
    EXPECT_FLOAT_EQ(a.y, 1.0f);
    EXPECT_FLOAT_EQ(a.z, 0.0f);
}

TEST(Vector3Statics, YAxisEquals3ArgInitCtor) {
    auto a = a2de::Vector3::Y_AXIS;
    auto b = a2de::Vector3{ 0.0f, 1.0f, 0.0f };
    EXPECT_FLOAT_EQ(a.x, b.x);
    EXPECT_FLOAT_EQ(a.y, b.y);
    EXPECT_FLOAT_EQ(a.z, b.z);
}

TEST(Vector3Statics, YZAxisSetsXToZeroYToOneZToOne) {
    auto a = a2de::Vector3::YZ_AXIS;
    EXPECT_FLOAT_EQ(a.x, 0.0f);
    EXPECT_FLOAT_EQ(a.y, 1.0f);
    EXPECT_FLOAT_EQ(a.z, 1.0f);
}

TEST(Vector3Statics, YZAxisEquals3ArgInitCtor) {
    auto a = a2de::Vector3::YZ_AXIS;
    auto b = a2de::Vector3{ 0.0f, 1.0f, 1.0f };
    EXPECT_FLOAT_EQ(a.x, b.x);
    EXPECT_FLOAT_EQ(a.y, b.y);
    EXPECT_FLOAT_EQ(a.z, b.z);
}

TEST(Vector3Statics, ZAxisSetsXToZeroYToZeroZToOne) {
    auto a = a2de::Vector3::Z_AXIS;
    EXPECT_FLOAT_EQ(a.x, 0.0f);
    EXPECT_FLOAT_EQ(a.y, 0.0f);
    EXPECT_FLOAT_EQ(a.z, 1.0f);
}

TEST(Vector3Statics, ZAxisEquals3ArgInitCtor) {
    auto a = a2de::Vector3::Z_AXIS;
    auto b = a2de::Vector3{ 0.0f, 0.0f, 1.0f };
    EXPECT_FLOAT_EQ(a.x, b.x);
    EXPECT_FLOAT_EQ(a.y, b.y);
    EXPECT_FLOAT_EQ(a.z, b.z);
}

TEST(Vector3Statics, OneSetsMembersToOne) {
    auto a = a2de::Vector3::ONE;
    EXPECT_FLOAT_EQ(a.x, 1.0f);
    EXPECT_FLOAT_EQ(a.y, 1.0f);
    EXPECT_FLOAT_EQ(a.z, 1.0f);
}

TEST(Vector3Statics, OneEquals3ArgInitCtor) {
    auto a = a2de::Vector3::ONE;
    auto b = a2de::Vector3{ 1.0f, 1.0f, 1.0f };
    EXPECT_FLOAT_EQ(a.x, b.x);
    EXPECT_FLOAT_EQ(a.y, b.y);
    EXPECT_FLOAT_EQ(a.z, b.z);
}

TEST(Vector3Construction, DefaultSetsMembersToZero) {
    auto a = a2de::Vector3{};
    EXPECT_FLOAT_EQ(a.x, 0.0f);
    EXPECT_FLOAT_EQ(a.y, 0.0f);
    EXPECT_FLOAT_EQ(a.z, 0.0f);
}

TEST(Vector3Construction, CopyCtor) {
    auto a = a2de::Vector3{ 1.0f, 2.0f, 3.0f };
    auto b = a2de::Vector3{ a };
    EXPECT_FLOAT_EQ(a.x, b.x);
    EXPECT_FLOAT_EQ(a.y, b.y);
    EXPECT_FLOAT_EQ(a.z, b.z);
}

TEST(Vector3Construction, MoveCtor) {
    auto a = a2de::Vector3{ 1.0f, 2.0f, 3.0f };
    auto b = a2de::Vector3{ std::move(a) };
    EXPECT_FLOAT_EQ(a.x, b.x);
    EXPECT_FLOAT_EQ(a.y, b.y);
    EXPECT_FLOAT_EQ(a.z, b.z);
}

TEST(Vector3Construction, CopyAssign) {
    auto a = a2de::Vector3{ 1.0f, 2.0f, 3.0f };
    auto b = a2de::Vector3{ 4.0f, 5.0f, 6.0f };
    b = a;
    EXPECT_FLOAT_EQ(b.x, a.x);
    EXPECT_FLOAT_EQ(b.y, a.y);
    EXPECT_FLOAT_EQ(b.z, a.z);
}

TEST(Vector3Construction, MoveAssign) {
    auto a = a2de::Vector3{ 1.0f, 2.0f, 3.0f };
    auto b = a2de::Vector3{ 4.0f, 5.0f, 6.0f };
    b = std::move(a);
    EXPECT_FLOAT_EQ(b.x, a.x);
    EXPECT_FLOAT_EQ(b.y, a.y);
    EXPECT_FLOAT_EQ(b.z, a.z);
}

TEST(Vector3Construction, FloatCtor) {
    auto a = a2de::Vector3{ 1.0f, 2.0f, 3.0f };
    EXPECT_FLOAT_EQ(a.x, 1.0f);
    EXPECT_FLOAT_EQ(a.y, 2.0f);
    EXPECT_FLOAT_EQ(a.z, 3.0f);
}

TEST(Vector3Construction, StringCtor) {
    auto a = a2de::Vector3{ std::string{"[3.0,4.0,5.0]"} };
    EXPECT_FLOAT_EQ(a.x, 3.0f);
    EXPECT_FLOAT_EQ(a.y, 4.0f);
    EXPECT_FLOAT_EQ(a.z, 5.0f);
    auto b = a2de::Vector3{ std::string{"[3,4,5]"} };
    EXPECT_FLOAT_EQ(b.x, 3.0f);
    EXPECT_FLOAT_EQ(b.y, 4.0f);
    EXPECT_FLOAT_EQ(b.z, 5.0f);
    auto c = a2de::Vector3{ std::string{"[3.1,4.2,5.3]"} };
    EXPECT_FLOAT_EQ(c.x, 3.1f);
    EXPECT_FLOAT_EQ(c.y, 4.2f);
    EXPECT_FLOAT_EQ(c.z, 5.3f);
}

TEST(Vector3Construction, Vector3Ctor) {
    auto a = a2de::Vector3{ a2de::Vector3{3.0f, 4.0f, 5.0f} };
    EXPECT_FLOAT_EQ(a.x, 3.0f);
    EXPECT_FLOAT_EQ(a.y, 4.0f);
    EXPECT_FLOAT_EQ(a.z, 5.0f);
}

TEST(Vector3Construction, IntVector3Ctor) {
    auto a = a2de::Vector3{ 1.3f, 2.7f, 3.8f };
    a = a2de::Vector3{ a2de::IntVector3{3, 4, 5} };
    EXPECT_FLOAT_EQ(a.x, 3.0f);
    EXPECT_FLOAT_EQ(a.y, 4.0f);
    EXPECT_FLOAT_EQ(a.z, 5.0f);
}

TEST(Vector3Operators, Equality) {
    auto a = a2de::Vector3{ 1.0f, 2.0f, 3.0f };
    auto b = a2de::Vector3{ 1.0f, 2.0f, 3.0f };
    auto c = a2de::Vector3{ 1.1f, 2.0f, 3.0f };
    auto d = a2de::Vector3{ 1.0f, 2.1f, 3.2f };
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a == c);
    EXPECT_FALSE(a == d);
}

TEST(Vector3Operators, Inequality) {
    auto a = a2de::Vector3{ 1.0f, 2.0f, 3.0f };
    auto b = a2de::Vector3{ 1.0f, 2.0f, 3.0f };
    auto c = a2de::Vector3{ 1.1f, 2.0f, 3.0f };
    auto d = a2de::Vector3{ 1.0f, 2.1f, 3.0f };
    EXPECT_FALSE(a != b);
    EXPECT_TRUE(a != c);
    EXPECT_TRUE(a != d);
}

TEST(Vector3Operators, Add) {
    auto a = a2de::Vector3{ 1.0f, 2.0f, 3.0f };
    auto b = a2de::Vector3::ONE;
    auto c = a + b;
    EXPECT_FLOAT_EQ(a.x + b.x, c.x);
    EXPECT_FLOAT_EQ(a.y + b.y, c.y);
    EXPECT_FLOAT_EQ(a.z + b.z, c.z);
}

TEST(Vector3Operators, AddAssign) {
    auto a = a2de::Vector3{ 1.0f, 2.0f, 3.0f };
    auto b = a2de::Vector3::ONE;
    a += b;
    EXPECT_FLOAT_EQ(a.x, 2.0f);
    EXPECT_FLOAT_EQ(a.y, 3.0f);
    EXPECT_FLOAT_EQ(a.z, 4.0f);
}

TEST(Vector3Operators, Subtract) {
    auto a = a2de::Vector3{ 1.0f, 2.0f, 3.0f };
    auto b = a2de::Vector3::ONE;
    auto c = a - b;
    EXPECT_FLOAT_EQ(a.x - b.x, c.x);
    EXPECT_FLOAT_EQ(a.y - b.y, c.y);
    EXPECT_FLOAT_EQ(a.z - b.z, c.z);
}

TEST(Vector3Operators, SubtractAssign) {
    auto a = a2de::Vector3{ 1.0f, 2.0f, 3.0f };
    auto b = a2de::Vector3::ONE;
    a -= b;
    EXPECT_FLOAT_EQ(a.x, 0.0f);
    EXPECT_FLOAT_EQ(a.y, 1.0f);
    EXPECT_FLOAT_EQ(a.z, 2.0f);
}

TEST(Vector3Operators, Negate) {
    auto a = a2de::Vector3{ 1.0f, 2.0f, 3.0f };
    auto b = -a;
    EXPECT_FLOAT_EQ(-a.x, b.x);
    EXPECT_FLOAT_EQ(-a.y, b.y);
    EXPECT_FLOAT_EQ(-a.z, b.z);
}

TEST(Vector3Operators, MultiplyRHSScalar) {
    auto a = a2de::Vector3::ONE;
    auto c = a * 2.0f;
    EXPECT_FLOAT_EQ(a.x * 2.0f, c.x);
    EXPECT_FLOAT_EQ(a.y * 2.0f, c.y);
    EXPECT_FLOAT_EQ(a.z * 2.0f, c.y);
}

TEST(Vector3Operators, MultiplyAssignScalar) {
    auto a = a2de::Vector3::ONE;
    a *= 2.0f;
    EXPECT_FLOAT_EQ(a.x, 2.0f);
    EXPECT_FLOAT_EQ(a.y, 2.0f);
    EXPECT_FLOAT_EQ(a.z, 2.0f);
}

TEST(Vector3Operators, MultiplyLHSScalar) {
    auto a = a2de::Vector3::ONE;
    auto c = 2.0f * a;
    EXPECT_FLOAT_EQ(a.x * 2.0f, c.x);
    EXPECT_FLOAT_EQ(a.y * 2.0f, c.y);
    EXPECT_FLOAT_EQ(a.z * 2.0f, c.z);
}

TEST(Vector3Operators, MultiplyLHSAndRHSScalar) {
    auto a = a2de::Vector3::ONE;
    auto c = 2.0f * a * 2.0f;
    EXPECT_FLOAT_EQ(a.x * 4.0f, c.x);
    EXPECT_FLOAT_EQ(a.y * 4.0f, c.y);
    EXPECT_FLOAT_EQ(a.z * 4.0f, c.z);
}

TEST(Vector3Operators, MultiplyVector3) {
    auto a = a2de::Vector3::ONE * 2.0f;
    auto b = a2de::Vector3{ 2.0f, 3.0f, 4.0f };
    auto c = a * b;
    EXPECT_FLOAT_EQ(a.x * b.x, c.x);
    EXPECT_FLOAT_EQ(a.y * b.y, c.y);
    EXPECT_FLOAT_EQ(a.z * b.z, c.z);
}

TEST(Vector3Operators, MultiplyAssignVector3) {
    auto a = a2de::Vector3{ 2.0f, 3.0f, 4.0f };
    auto b = a2de::Vector3{ 2.0f, 2.0f, 2.0f };
    a *= b;
    EXPECT_FLOAT_EQ(a.x, 2.0f * 2.0f);
    EXPECT_FLOAT_EQ(a.y, 3.0f * 2.0f);
    EXPECT_FLOAT_EQ(a.z, 4.0f * 2.0f);
}

TEST(Vector3Operators, DivideRHSScalar) {
    auto a = a2de::Vector3::ONE;
    auto c = a / 2.0f;
    EXPECT_FLOAT_EQ(a.x / 2.0f, c.x);
    EXPECT_FLOAT_EQ(a.y / 2.0f, c.y);
    EXPECT_FLOAT_EQ(a.z / 2.0f, c.z);
}

TEST(Vector3Operators, DivideAssignScalar) {
    auto a = a2de::Vector3::ONE;
    a /= 2.0f;
    EXPECT_FLOAT_EQ(a.x, 1.0f / 2.0f);
    EXPECT_FLOAT_EQ(a.y, 1.0f / 2.0f);
    EXPECT_FLOAT_EQ(a.z, 1.0f / 2.0f);
}

TEST(Vector3Operators, DivideVector3) {
    auto a = a2de::Vector3::ONE * 2.0f;
    auto b = a2de::Vector3{ 2.0f, 3.0f, 4.0f };
    auto c = a / b;
    EXPECT_FLOAT_EQ(a.x / b.x, c.x);
    EXPECT_FLOAT_EQ(a.y / b.y, c.y);
    EXPECT_FLOAT_EQ(a.z / b.z, c.z);
}

TEST(Vector3Operators, DivideAssignVector3) {
    auto a = a2de::Vector3{ 2.0f, 3.0f, 4.0f };
    auto b = a2de::Vector3{ 2.0f, 2.0f, 2.0f };
    a /= b;
    EXPECT_FLOAT_EQ(a.x, 2.0f / 2.0f);
    EXPECT_FLOAT_EQ(a.y, 3.0f / 2.0f);
    EXPECT_FLOAT_EQ(a.z, 4.0f / 2.0f);
}

TEST(Vector3Operators, StreamIn) {
    auto a = a2de::Vector3{};
    {
        std::istringstream ss;
        ss.str("[1.0,2.0,3.0]");
        ss.clear();
        ss.seekg(0);
        ss >> a;
        EXPECT_FLOAT_EQ(a.x, 1.0f);
        EXPECT_FLOAT_EQ(a.y, 2.0f);
        EXPECT_FLOAT_EQ(a.z, 3.0f);
    }
    {
        std::istringstream ss;
        ss.str("[4,3,2]");
        ss.clear();
        ss.seekg(0);
        ss >> a;
        EXPECT_FLOAT_EQ(a.x, 4.0f);
        EXPECT_FLOAT_EQ(a.y, 3.0f);
        EXPECT_FLOAT_EQ(a.z, 2.0f);
    }
}

TEST(Vector3Operators, StreamOut) {
    auto a = a2de::Vector3{ 1.0f, 2.0f, 3.0f };
    std::ostringstream ss;
    ss << a;
    EXPECT_FLOAT_EQ(a.x, 1.0f);
    EXPECT_FLOAT_EQ(a.y, 2.0f);
    EXPECT_FLOAT_EQ(a.z, 3.0f);
    EXPECT_STREQ(ss.str().c_str(), "[1,2,3]");
    ss.str("");
    a = a2de::Vector3{ 1.1f, 2.2f, 3.3f };
    ss << a;
    EXPECT_FLOAT_EQ(a.x, 1.1f);
    EXPECT_FLOAT_EQ(a.y, 2.2f);
    EXPECT_FLOAT_EQ(a.z, 3.3f);
    EXPECT_STREQ(ss.str().c_str(), "[1.1,2.2,3.3]");
}
