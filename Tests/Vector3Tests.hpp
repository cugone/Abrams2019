#pragma once

#include "pch.h"

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/IntVector3.hpp"
#include "Engine/Math/Vector3.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

TEST(Vector3Statics, ZeroSetsMembersToZero) {
    auto a = Vector3::Zero;
    EXPECT_FLOAT_EQ(a.x, 0.0f);
    EXPECT_FLOAT_EQ(a.y, 0.0f);
    EXPECT_FLOAT_EQ(a.z, 0.0f);
}

TEST(Vector3Statics, ZeroEqualsDefaultCtor) {
    auto a = Vector3::Zero;
    auto b = Vector3{};
    EXPECT_FLOAT_EQ(a.x, b.x);
    EXPECT_FLOAT_EQ(a.y, b.y);
    EXPECT_FLOAT_EQ(a.z, b.z);
}

TEST(Vector3Statics, ZeroEquals3ArgInitCtor) {
    auto a = Vector3::Zero;
    auto b = Vector3{ 0.0f, 0.0f, 0.0f };
    EXPECT_FLOAT_EQ(a.x, b.x);
    EXPECT_FLOAT_EQ(a.y, b.y);
    EXPECT_FLOAT_EQ(a.z, b.z);
}

TEST(Vector3Statics, XAxisSetsXToOneYToZeroZToZero) {
    auto a = Vector3::X_Axis;
    EXPECT_FLOAT_EQ(a.x, 1.0f);
    EXPECT_FLOAT_EQ(a.y, 0.0f);
    EXPECT_FLOAT_EQ(a.z, 0.0f);
}

TEST(Vector3Statics, XAxisEquals3ArgInitCtor) {
    auto a = Vector3::X_Axis;
    auto b = Vector3{ 1.0f, 0.0f, 0.0f };
    EXPECT_FLOAT_EQ(a.x, b.x);
    EXPECT_FLOAT_EQ(a.y, b.y);
    EXPECT_FLOAT_EQ(a.z, b.z);
}

TEST(Vector3Statics, XYAxisSetsXToOneYToOneZToZero) {
    auto a = Vector3::XY_Axis;
    EXPECT_FLOAT_EQ(a.x, 1.0f);
    EXPECT_FLOAT_EQ(a.y, 1.0f);
    EXPECT_FLOAT_EQ(a.z, 0.0f);
}

TEST(Vector3Statics, XYAxisEquals3ArgInitCtor) {
    auto a = Vector3::XY_Axis;
    auto b = Vector3{ 1.0f, 1.0f, 0.0f };
    EXPECT_FLOAT_EQ(a.x, b.x);
    EXPECT_FLOAT_EQ(a.y, b.y);
    EXPECT_FLOAT_EQ(a.z, b.z);
}

TEST(Vector3Statics, XZAxisSetsXToOneYToZeroZToOne) {
    auto a = Vector3::XZ_Axis;
    EXPECT_FLOAT_EQ(a.x, 1.0f);
    EXPECT_FLOAT_EQ(a.y, 0.0f);
    EXPECT_FLOAT_EQ(a.z, 1.0f);
}

TEST(Vector3Statics, XZAxisEquals3ArgInitCtor) {
    auto a = Vector3::XZ_Axis;
    auto b = Vector3{ 1.0f, 0.0f, 1.0f };
    EXPECT_FLOAT_EQ(a.x, b.x);
    EXPECT_FLOAT_EQ(a.y, b.y);
    EXPECT_FLOAT_EQ(a.z, b.z);
}

TEST(Vector3Statics, YAxisSetsXToZeroYToOneZToZero) {
    auto a = Vector3::Y_Axis;
    EXPECT_FLOAT_EQ(a.x, 0.0f);
    EXPECT_FLOAT_EQ(a.y, 1.0f);
    EXPECT_FLOAT_EQ(a.z, 0.0f);
}

TEST(Vector3Statics, YAxisEquals3ArgInitCtor) {
    auto a = Vector3::Y_Axis;
    auto b = Vector3{ 0.0f, 1.0f, 0.0f };
    EXPECT_FLOAT_EQ(a.x, b.x);
    EXPECT_FLOAT_EQ(a.y, b.y);
    EXPECT_FLOAT_EQ(a.z, b.z);
}

TEST(Vector3Statics, YZAxisSetsXToZeroYToOneZToOne) {
    auto a = Vector3::YZ_Axis;
    EXPECT_FLOAT_EQ(a.x, 0.0f);
    EXPECT_FLOAT_EQ(a.y, 1.0f);
    EXPECT_FLOAT_EQ(a.z, 1.0f);
}

TEST(Vector3Statics, YZAxisEquals3ArgInitCtor) {
    auto a = Vector3::YZ_Axis;
    auto b = Vector3{ 0.0f, 1.0f, 1.0f };
    EXPECT_FLOAT_EQ(a.x, b.x);
    EXPECT_FLOAT_EQ(a.y, b.y);
    EXPECT_FLOAT_EQ(a.z, b.z);
}

TEST(Vector3Statics, ZAxisSetsXToZeroYToZeroZToOne) {
    auto a = Vector3::Z_Axis;
    EXPECT_FLOAT_EQ(a.x, 0.0f);
    EXPECT_FLOAT_EQ(a.y, 0.0f);
    EXPECT_FLOAT_EQ(a.z, 1.0f);
}

TEST(Vector3Statics, ZAxisEquals3ArgInitCtor) {
    auto a = Vector3::Z_Axis;
    auto b = Vector3{ 0.0f, 0.0f, 1.0f };
    EXPECT_FLOAT_EQ(a.x, b.x);
    EXPECT_FLOAT_EQ(a.y, b.y);
    EXPECT_FLOAT_EQ(a.z, b.z);
}

TEST(Vector3Statics, OneSetsMembersToOne) {
    auto a = Vector3::One;
    EXPECT_FLOAT_EQ(a.x, 1.0f);
    EXPECT_FLOAT_EQ(a.y, 1.0f);
    EXPECT_FLOAT_EQ(a.z, 1.0f);
}

TEST(Vector3Statics, OneEquals3ArgInitCtor) {
    auto a = Vector3::One;
    auto b = Vector3{ 1.0f, 1.0f, 1.0f };
    EXPECT_FLOAT_EQ(a.x, b.x);
    EXPECT_FLOAT_EQ(a.y, b.y);
    EXPECT_FLOAT_EQ(a.z, b.z);
}

TEST(Vector3Construction, DefaultSetsMembersToZero) {
    auto a = Vector3{};
    EXPECT_FLOAT_EQ(a.x, 0.0f);
    EXPECT_FLOAT_EQ(a.y, 0.0f);
    EXPECT_FLOAT_EQ(a.z, 0.0f);
}

TEST(Vector3Construction, CopyCtor) {
    auto a = Vector3{ 1.0f, 2.0f, 3.0f };
    auto b = Vector3{ a };
    EXPECT_FLOAT_EQ(a.x, b.x);
    EXPECT_FLOAT_EQ(a.y, b.y);
    EXPECT_FLOAT_EQ(a.z, b.z);
}

TEST(Vector3Construction, MoveCtor) {
    auto a = Vector3{ 1.0f, 2.0f, 3.0f };
    Vector3 b{std::move(a)};
    EXPECT_FLOAT_EQ(a.x, b.x);
    EXPECT_FLOAT_EQ(a.y, b.y);
    EXPECT_FLOAT_EQ(a.z, b.z);
}

TEST(Vector3Construction, CopyAssign) {
    auto a = Vector3{ 1.0f, 2.0f, 3.0f };
    auto b = Vector3{ 4.0f, 5.0f, 6.0f };
    b = a;
    EXPECT_FLOAT_EQ(b.x, a.x);
    EXPECT_FLOAT_EQ(b.y, a.y);
    EXPECT_FLOAT_EQ(b.z, a.z);
}

TEST(Vector3Construction, MoveAssign) {
    auto a = Vector3{ 1.0f, 2.0f, 3.0f };
    auto b = Vector3{ 4.0f, 5.0f, 6.0f };
    b = std::move(a);
    EXPECT_FLOAT_EQ(b.x, a.x);
    EXPECT_FLOAT_EQ(b.y, a.y);
    EXPECT_FLOAT_EQ(b.z, a.z);
}

TEST(Vector3Construction, FloatCtor) {
    auto a = Vector3{ 1.0f, 2.0f, 3.0f };
    EXPECT_FLOAT_EQ(a.x, 1.0f);
    EXPECT_FLOAT_EQ(a.y, 2.0f);
    EXPECT_FLOAT_EQ(a.z, 3.0f);
}

TEST(Vector3Construction, StringCtor) {
    auto a = Vector3{ std::string{"[3.0,4.0,5.0]"} };
    EXPECT_FLOAT_EQ(a.x, 3.0f);
    EXPECT_FLOAT_EQ(a.y, 4.0f);
    EXPECT_FLOAT_EQ(a.z, 5.0f);
    auto b = Vector3{ std::string{"[3,4,5]"} };
    EXPECT_FLOAT_EQ(b.x, 3.0f);
    EXPECT_FLOAT_EQ(b.y, 4.0f);
    EXPECT_FLOAT_EQ(b.z, 5.0f);
    auto c = Vector3{ std::string{"[3.1,4.2,5.3]"} };
    EXPECT_FLOAT_EQ(c.x, 3.1f);
    EXPECT_FLOAT_EQ(c.y, 4.2f);
    EXPECT_FLOAT_EQ(c.z, 5.3f);
}

TEST(Vector3Construction, Vector3Ctor) {
    auto a = Vector3{ Vector3{3.0f, 4.0f, 5.0f} };
    EXPECT_FLOAT_EQ(a.x, 3.0f);
    EXPECT_FLOAT_EQ(a.y, 4.0f);
    EXPECT_FLOAT_EQ(a.z, 5.0f);
}

TEST(Vector3Construction, IntVector3Ctor) {
    auto a = Vector3{ 1.3f, 2.7f, 3.8f };
    a = Vector3{ IntVector3{3, 4, 5} };
    EXPECT_FLOAT_EQ(a.x, 3.0f);
    EXPECT_FLOAT_EQ(a.y, 4.0f);
    EXPECT_FLOAT_EQ(a.z, 5.0f);
}

TEST(Vector3Operators, Equality) {
    auto a = Vector3{ 1.0f, 2.0f, 3.0f };
    auto b = Vector3{ 1.0f, 2.0f, 3.0f };
    auto c = Vector3{ 1.1f, 2.0f, 3.0f };
    auto d = Vector3{ 1.0f, 2.1f, 3.2f };
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a == c);
    EXPECT_FALSE(a == d);
}

TEST(Vector3Operators, Inequality) {
    auto a = Vector3{ 1.0f, 2.0f, 3.0f };
    auto b = Vector3{ 1.0f, 2.0f, 3.0f };
    auto c = Vector3{ 1.1f, 2.0f, 3.0f };
    auto d = Vector3{ 1.0f, 2.1f, 3.0f };
    EXPECT_FALSE(a != b);
    EXPECT_TRUE(a != c);
    EXPECT_TRUE(a != d);
}

TEST(Vector3Operators, Add) {
    auto a = Vector3{ 1.0f, 2.0f, 3.0f };
    auto b = Vector3::One;
    auto c = a + b;
    EXPECT_FLOAT_EQ(a.x + b.x, c.x);
    EXPECT_FLOAT_EQ(a.y + b.y, c.y);
    EXPECT_FLOAT_EQ(a.z + b.z, c.z);
}

TEST(Vector3Operators, AddAssign) {
    auto a = Vector3{ 1.0f, 2.0f, 3.0f };
    auto b = Vector3::One;
    a += b;
    EXPECT_FLOAT_EQ(a.x, 2.0f);
    EXPECT_FLOAT_EQ(a.y, 3.0f);
    EXPECT_FLOAT_EQ(a.z, 4.0f);
}

TEST(Vector3Operators, Subtract) {
    auto a = Vector3{ 1.0f, 2.0f, 3.0f };
    auto b = Vector3::One;
    auto c = a - b;
    EXPECT_FLOAT_EQ(a.x - b.x, c.x);
    EXPECT_FLOAT_EQ(a.y - b.y, c.y);
    EXPECT_FLOAT_EQ(a.z - b.z, c.z);
}

TEST(Vector3Operators, SubtractAssign) {
    auto a = Vector3{ 1.0f, 2.0f, 3.0f };
    auto b = Vector3::One;
    a -= b;
    EXPECT_FLOAT_EQ(a.x, 0.0f);
    EXPECT_FLOAT_EQ(a.y, 1.0f);
    EXPECT_FLOAT_EQ(a.z, 2.0f);
}

TEST(Vector3Operators, Negate) {
    auto a = Vector3{ 1.0f, 2.0f, 3.0f };
    auto b = -a;
    EXPECT_FLOAT_EQ(-a.x, b.x);
    EXPECT_FLOAT_EQ(-a.y, b.y);
    EXPECT_FLOAT_EQ(-a.z, b.z);
}

TEST(Vector3Operators, MultiplyRHSScalar) {
    auto a = Vector3::One;
    auto c = a * 2.0f;
    EXPECT_FLOAT_EQ(a.x * 2.0f, c.x);
    EXPECT_FLOAT_EQ(a.y * 2.0f, c.y);
    EXPECT_FLOAT_EQ(a.z * 2.0f, c.y);
}

TEST(Vector3Operators, MultiplyAssignScalar) {
    auto a = Vector3::One;
    a *= 2.0f;
    EXPECT_FLOAT_EQ(a.x, 2.0f);
    EXPECT_FLOAT_EQ(a.y, 2.0f);
    EXPECT_FLOAT_EQ(a.z, 2.0f);
}

TEST(Vector3Operators, MultiplyLHSScalar) {
    auto a = Vector3::One;
    auto c = 2.0f * a;
    EXPECT_FLOAT_EQ(a.x * 2.0f, c.x);
    EXPECT_FLOAT_EQ(a.y * 2.0f, c.y);
    EXPECT_FLOAT_EQ(a.z * 2.0f, c.z);
}

TEST(Vector3Operators, MultiplyLHSAndRHSScalar) {
    auto a = Vector3::One;
    auto c = 2.0f * a * 2.0f;
    EXPECT_FLOAT_EQ(a.x * 4.0f, c.x);
    EXPECT_FLOAT_EQ(a.y * 4.0f, c.y);
    EXPECT_FLOAT_EQ(a.z * 4.0f, c.z);
}

TEST(Vector3Operators, MultiplyVector3) {
    auto a = Vector3::One * 2.0f;
    auto b = Vector3{ 2.0f, 3.0f, 4.0f };
    auto c = a * b;
    EXPECT_FLOAT_EQ(a.x * b.x, c.x);
    EXPECT_FLOAT_EQ(a.y * b.y, c.y);
    EXPECT_FLOAT_EQ(a.z * b.z, c.z);
}

TEST(Vector3Operators, MultiplyAssignVector3) {
    auto a = Vector3{ 2.0f, 3.0f, 4.0f };
    auto b = Vector3{ 2.0f, 2.0f, 2.0f };
    a *= b;
    EXPECT_FLOAT_EQ(a.x, 2.0f * 2.0f);
    EXPECT_FLOAT_EQ(a.y, 3.0f * 2.0f);
    EXPECT_FLOAT_EQ(a.z, 4.0f * 2.0f);
}

TEST(Vector3Operators, DivideRHSScalar) {
    auto a = Vector3::One;
    auto c = a / 2.0f;
    EXPECT_FLOAT_EQ(a.x / 2.0f, c.x);
    EXPECT_FLOAT_EQ(a.y / 2.0f, c.y);
    EXPECT_FLOAT_EQ(a.z / 2.0f, c.z);
}

TEST(Vector3Operators, DivideAssignScalar) {
    auto a = Vector3::One;
    a /= 2.0f;
    EXPECT_FLOAT_EQ(a.x, 1.0f / 2.0f);
    EXPECT_FLOAT_EQ(a.y, 1.0f / 2.0f);
    EXPECT_FLOAT_EQ(a.z, 1.0f / 2.0f);
}

TEST(Vector3Operators, DivideVector3) {
    auto a = Vector3::One * 2.0f;
    auto b = Vector3{ 2.0f, 3.0f, 4.0f };
    auto c = a / b;
    EXPECT_FLOAT_EQ(a.x / b.x, c.x);
    EXPECT_FLOAT_EQ(a.y / b.y, c.y);
    EXPECT_FLOAT_EQ(a.z / b.z, c.z);
}

TEST(Vector3Operators, DivideAssignVector3) {
    auto a = Vector3{ 2.0f, 3.0f, 4.0f };
    auto b = Vector3{ 2.0f, 2.0f, 2.0f };
    a /= b;
    EXPECT_FLOAT_EQ(a.x, 2.0f / 2.0f);
    EXPECT_FLOAT_EQ(a.y, 3.0f / 2.0f);
    EXPECT_FLOAT_EQ(a.z, 4.0f / 2.0f);
}

TEST(Vector3Operators, StreamIn) {
    auto a = Vector3{};
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
    auto a = Vector3{ 1.0f, 2.0f, 3.0f };
    std::ostringstream ss;
    ss << a;
    EXPECT_FLOAT_EQ(a.x, 1.0f);
    EXPECT_FLOAT_EQ(a.y, 2.0f);
    EXPECT_FLOAT_EQ(a.z, 3.0f);
    EXPECT_STREQ(ss.str().c_str(), "[1,2,3]");
    ss.str("");
    a = Vector3{ 1.1f, 2.2f, 3.3f };
    ss << a;
    EXPECT_FLOAT_EQ(a.x, 1.1f);
    EXPECT_FLOAT_EQ(a.y, 2.2f);
    EXPECT_FLOAT_EQ(a.z, 3.3f);
    EXPECT_STREQ(ss.str().c_str(), "[1.1,2.2,3.3]");
}
