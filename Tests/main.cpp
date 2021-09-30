#include "pch.h"

#include "Vector2Tests.hpp"

#include "Vector3Tests.hpp"

#include "MathUtilsTests.hpp"

#include "StringUtilsTest.hpp"

#include "UuidTests.hpp"

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}