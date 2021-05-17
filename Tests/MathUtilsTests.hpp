#pragma once

#include "pch.h"

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Quaternion.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

TEST(MathUtilsFunctions, SplitFloatingPointValue) {
    auto a = 1.2f;
    auto rf = MathUtils::SplitFloatingPointValue(a);
    EXPECT_FLOAT_EQ(rf.first, 1.0f);
    EXPECT_FLOAT_EQ(rf.second, 0.2f);
    auto d = 1.2;
    auto rd = MathUtils::SplitFloatingPointValue(d);
    EXPECT_DOUBLE_EQ(rd.first, 1.0);
    EXPECT_DOUBLE_EQ(rd.second, 0.2);
    auto ld = 1.2l;
    auto rld = MathUtils::SplitFloatingPointValue(ld);
    EXPECT_DOUBLE_EQ(static_cast<double>(rld.first), static_cast<double>(1.0l));
    EXPECT_DOUBLE_EQ(static_cast<double>(rld.second),static_cast<double>(0.2l));
}

TEST(MathUtilsFunctions, IsEquivalentToZero) {
    auto a = 0.0f;
    auto b = -0.0f;
    auto c = -0.1f;
    auto d = 0.1f;
    auto e = 1.0f;
    auto f = -1.0f;
    auto g = 1.1f;
    auto h = -1.1f;
    EXPECT_TRUE(MathUtils::IsEquivalentToZero(a));
    EXPECT_TRUE(MathUtils::IsEquivalentToZero(b));
    EXPECT_FALSE(MathUtils::IsEquivalentToZero(c));
    EXPECT_FALSE(MathUtils::IsEquivalentToZero(d));
    EXPECT_FALSE(MathUtils::IsEquivalentToZero(e));
    EXPECT_FALSE(MathUtils::IsEquivalentToZero(f));
    EXPECT_FALSE(MathUtils::IsEquivalentToZero(g));
    EXPECT_FALSE(MathUtils::IsEquivalentToZero(h));
}

TEST(MathUtilsFunctions, IsEquivalentOrLessThan) {
    auto a = -2.0f;
    EXPECT_TRUE(MathUtils::IsEquivalentOrLessThan(a, -1.1f));
    EXPECT_TRUE(MathUtils::IsEquivalentOrLessThan(a, -1.0f));
    EXPECT_TRUE(MathUtils::IsEquivalentOrLessThan(a, 0.0f));
    EXPECT_TRUE(MathUtils::IsEquivalentOrLessThan(a, 1.0f));
    EXPECT_TRUE(MathUtils::IsEquivalentOrLessThan(a, 1.1f));
    a = -1.0f;
    EXPECT_FALSE(MathUtils::IsEquivalentOrLessThan(a, -1.1f));
    EXPECT_TRUE(MathUtils::IsEquivalentOrLessThan(a, -1.0f));
    EXPECT_TRUE(MathUtils::IsEquivalentOrLessThan(a, 0.0f));
    EXPECT_TRUE(MathUtils::IsEquivalentOrLessThan(a, 1.0f));
    EXPECT_TRUE(MathUtils::IsEquivalentOrLessThan(a, 1.1f));
    a = 0.0f;
    EXPECT_FALSE(MathUtils::IsEquivalentOrLessThan(a, -1.1f));
    EXPECT_FALSE(MathUtils::IsEquivalentOrLessThan(a, -1.0f));
    EXPECT_TRUE(MathUtils::IsEquivalentOrLessThan(a, 0.0f));
    EXPECT_TRUE(MathUtils::IsEquivalentOrLessThan(a, 1.0f));
    EXPECT_TRUE(MathUtils::IsEquivalentOrLessThan(a, 1.1f));
    a = 1.0f;
    EXPECT_FALSE(MathUtils::IsEquivalentOrLessThan(a, -1.1f));
    EXPECT_FALSE(MathUtils::IsEquivalentOrLessThan(a, -1.0f));
    EXPECT_FALSE(MathUtils::IsEquivalentOrLessThan(a, 0.0f));
    EXPECT_TRUE(MathUtils::IsEquivalentOrLessThan(a, 1.0f));
    EXPECT_TRUE(MathUtils::IsEquivalentOrLessThan(a, 1.1f));
    a = 2.0f;
    EXPECT_FALSE(MathUtils::IsEquivalentOrLessThan(a, 0.0f));
    EXPECT_FALSE(MathUtils::IsEquivalentOrLessThan(a, 1.0f));
    EXPECT_FALSE(MathUtils::IsEquivalentOrLessThan(a, 1.1f));
}

TEST(MathUtilsFunctions, IsEquivalent) {
    auto a = 1.0f;
    EXPECT_TRUE(MathUtils::IsEquivalent(a, 1.0f));
    EXPECT_FALSE(MathUtils::IsEquivalent(a, 0.0f));
    EXPECT_FALSE(MathUtils::IsEquivalent(a, 1.1f));
    auto b = 1.0;
    EXPECT_TRUE(MathUtils::IsEquivalent(b, 1.0));
    EXPECT_FALSE(MathUtils::IsEquivalent(b, 0.0));
    EXPECT_FALSE(MathUtils::IsEquivalent(b, 1.1));
    auto c = 1.0L;
    EXPECT_TRUE(MathUtils::IsEquivalent(c, 1.0L));
    EXPECT_FALSE(MathUtils::IsEquivalent(c, 0.0L));
    EXPECT_FALSE(MathUtils::IsEquivalent(c, 1.1L));
    auto d = Vector2{1.0f, 1.0f};
    EXPECT_TRUE(MathUtils::IsEquivalent(d,  Vector2::ONE));
    EXPECT_FALSE(MathUtils::IsEquivalent(d, Vector2::ZERO));
    EXPECT_FALSE(MathUtils::IsEquivalent(d, Vector2::X_AXIS));
    EXPECT_FALSE(MathUtils::IsEquivalent(d, Vector2::Y_AXIS));
    EXPECT_TRUE(MathUtils::IsEquivalent(d, Vector2::XY_AXIS));
    EXPECT_TRUE(MathUtils::IsEquivalent(d, Vector2::YX_AXIS));
    auto e = Vector3{1.0f, 1.0f, 1.0f};
    EXPECT_TRUE(MathUtils::IsEquivalent(e,  Vector3::ONE));
    EXPECT_FALSE(MathUtils::IsEquivalent(e, Vector3::ZERO));
    EXPECT_FALSE(MathUtils::IsEquivalent(e, Vector3::XY_AXIS));
    EXPECT_FALSE(MathUtils::IsEquivalent(e, Vector3::XZ_AXIS));
    EXPECT_FALSE(MathUtils::IsEquivalent(e, Vector3::YZ_AXIS));
    EXPECT_FALSE(MathUtils::IsEquivalent(e, Vector3::Z_AXIS));
    EXPECT_FALSE(MathUtils::IsEquivalent(e, Vector3::Y_AXIS));
    EXPECT_FALSE(MathUtils::IsEquivalent(e, Vector3::X_AXIS));
    auto f = Vector4{1.0f, 1.0f, 1.0f, 1.0f};
    EXPECT_TRUE(MathUtils::IsEquivalent(f,  Vector4::ONE));
    EXPECT_FALSE(MathUtils::IsEquivalent(f, Vector4::X_AXIS));
    EXPECT_FALSE(MathUtils::IsEquivalent(f, Vector4::XW_AXIS));
    EXPECT_FALSE(MathUtils::IsEquivalent(f, Vector4::XY_AXIS));
    EXPECT_FALSE(MathUtils::IsEquivalent(f, Vector4::XZ_AXIS));
    EXPECT_FALSE(MathUtils::IsEquivalent(f, Vector4::Y_AXIS));
    EXPECT_FALSE(MathUtils::IsEquivalent(f, Vector4::YW_AXIS));
    EXPECT_FALSE(MathUtils::IsEquivalent(f, Vector4::YX_AXIS));
    EXPECT_FALSE(MathUtils::IsEquivalent(f, Vector4::YZ_AXIS));
    EXPECT_FALSE(MathUtils::IsEquivalent(f, Vector4::Z_AXIS));
    EXPECT_FALSE(MathUtils::IsEquivalent(f, Vector4::ZW_AXIS));
    EXPECT_FALSE(MathUtils::IsEquivalent(f, Vector4::ZX_AXIS));
    EXPECT_FALSE(MathUtils::IsEquivalent(f, Vector4::ZY_AXIS));
    EXPECT_FALSE(MathUtils::IsEquivalent(f, Vector4::W_AXIS));
    EXPECT_FALSE(MathUtils::IsEquivalent(f, Vector4::WX_AXIS));
    EXPECT_FALSE(MathUtils::IsEquivalent(f, Vector4::WY_AXIS));
    EXPECT_FALSE(MathUtils::IsEquivalent(f, Vector4::WZ_AXIS));
    EXPECT_FALSE(MathUtils::IsEquivalent(f, Vector4::XYZ_AXIS));
    auto g = Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
    EXPECT_TRUE(MathUtils::IsEquivalent(g, Quaternion(1.0f, 0.0f, 0.0f, 0.0f)));
    EXPECT_FALSE(MathUtils::IsEquivalent(g, Quaternion(1.0f, 0.0f, 0.0f, 1.0f)));
}

TEST(MathUtilsFunctions, ConvertDegreesToRadians) {
    EXPECT_FLOAT_EQ(MathUtils::ConvertDegreesToRadians(0.0f), 0.0f);
    EXPECT_FLOAT_EQ(MathUtils::ConvertDegreesToRadians(30.0f), MathUtils::M_1PI_6);
    EXPECT_FLOAT_EQ(MathUtils::ConvertDegreesToRadians(45.0f), MathUtils::M_1PI_4);
    EXPECT_FLOAT_EQ(MathUtils::ConvertDegreesToRadians(60.0f), MathUtils::M_1PI_3);
    EXPECT_FLOAT_EQ(MathUtils::ConvertDegreesToRadians(90.0f), MathUtils::M_1PI_2);
    EXPECT_FLOAT_EQ(MathUtils::ConvertDegreesToRadians(120.0f), MathUtils::M_2PI_3);
    EXPECT_FLOAT_EQ(MathUtils::ConvertDegreesToRadians(135.0f), MathUtils::M_3PI_4);
    EXPECT_FLOAT_EQ(MathUtils::ConvertDegreesToRadians(150.0f), MathUtils::M_5PI_6);
    EXPECT_FLOAT_EQ(MathUtils::ConvertDegreesToRadians(180.0f), MathUtils::M_PI);
    EXPECT_FLOAT_EQ(MathUtils::ConvertDegreesToRadians(210.0f), MathUtils::M_7PI_6);
    EXPECT_FLOAT_EQ(MathUtils::ConvertDegreesToRadians(225.0f), MathUtils::M_5PI_4);
    EXPECT_FLOAT_EQ(MathUtils::ConvertDegreesToRadians(240.0f), MathUtils::M_4PI_3);
    EXPECT_FLOAT_EQ(MathUtils::ConvertDegreesToRadians(270.0f), MathUtils::M_3PI_2);
    EXPECT_FLOAT_EQ(MathUtils::ConvertDegreesToRadians(300.0f), MathUtils::M_5PI_3);
    EXPECT_FLOAT_EQ(MathUtils::ConvertDegreesToRadians(315.0f), MathUtils::M_7PI_4);
    EXPECT_FLOAT_EQ(MathUtils::ConvertDegreesToRadians(330.0f), MathUtils::M_11PI_6);
    EXPECT_FLOAT_EQ(MathUtils::ConvertDegreesToRadians(360.0f), MathUtils::M_2PI);
}

TEST(MathUtilsFunctions, ConvertRadiansToDegrees) {
    EXPECT_FLOAT_EQ(MathUtils::ConvertRadiansToDegrees(0.0f), 0.0f);
    EXPECT_FLOAT_EQ(MathUtils::ConvertRadiansToDegrees(MathUtils::M_1PI_6), 30.0f);
    EXPECT_FLOAT_EQ(MathUtils::ConvertRadiansToDegrees(MathUtils::M_1PI_4), 45.0f);
    EXPECT_FLOAT_EQ(MathUtils::ConvertRadiansToDegrees(MathUtils::M_1PI_3), 60.0f);
    EXPECT_FLOAT_EQ(MathUtils::ConvertRadiansToDegrees(MathUtils::M_1PI_2), 90.0f);
    EXPECT_FLOAT_EQ(MathUtils::ConvertRadiansToDegrees(MathUtils::M_2PI_3), 120.0f);
    EXPECT_FLOAT_EQ(MathUtils::ConvertRadiansToDegrees(MathUtils::M_3PI_4), 135.0f);
    EXPECT_FLOAT_EQ(MathUtils::ConvertRadiansToDegrees(MathUtils::M_5PI_6), 150.0f);
    EXPECT_FLOAT_EQ(MathUtils::ConvertRadiansToDegrees(MathUtils::M_PI), 180.0f);
    EXPECT_FLOAT_EQ(MathUtils::ConvertRadiansToDegrees(MathUtils::M_7PI_6), 210.0f);
    EXPECT_FLOAT_EQ(MathUtils::ConvertRadiansToDegrees(MathUtils::M_5PI_4), 225.0f);
    EXPECT_FLOAT_EQ(MathUtils::ConvertRadiansToDegrees(MathUtils::M_4PI_3), 240.0f);
    EXPECT_FLOAT_EQ(MathUtils::ConvertRadiansToDegrees(MathUtils::M_3PI_2), 270.0f);
    EXPECT_FLOAT_EQ(MathUtils::ConvertRadiansToDegrees(MathUtils::M_5PI_3), 300.0f);
    EXPECT_FLOAT_EQ(MathUtils::ConvertRadiansToDegrees(MathUtils::M_7PI_4), 315.0f);
    EXPECT_FLOAT_EQ(MathUtils::ConvertRadiansToDegrees(MathUtils::M_11PI_6), 330.0f);
    EXPECT_FLOAT_EQ(MathUtils::ConvertRadiansToDegrees(MathUtils::M_2PI), 360.0f);
}
