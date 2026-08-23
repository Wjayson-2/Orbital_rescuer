#include <gtest/gtest.h>

#include <numbers>

#include "WrapAngle.hpp"

TEST(WrapAngleTest, LeavesZeroUnchanged) {
    EXPECT_DOUBLE_EQ(wrapAngle(0.0), 0.0);
}

TEST(WrapAngleTest, WrapsAbovePi) {
    EXPECT_NEAR(
        wrapAngle(3.5),
        3.5 - 2.0 * std::numbers::pi,
        1e-9
    );
}

TEST(WrapAngleTest, WrapsBelowNegativePi) {
    EXPECT_NEAR(
        wrapAngle(-3.5),
        -3.5 + 2.0 * std::numbers::pi,
        1e-9
    );
}