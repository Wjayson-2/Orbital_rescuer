#include <gtest/gtest.h>

#include "Vec2.hpp"

TEST(Vec2Test, MagnitudeOfThreeFourVectorIsFive) {
    Vec2 v{3.0, 4.0};

    EXPECT_DOUBLE_EQ(v.magnitude(), 5.0);
}

TEST(Vec2Test, AdditionWorksCorrectly) {
    Vec2 a{1.0, 2.0};
    Vec2 b{3.0, 4.0};

    Vec2 result = a + b;

    EXPECT_DOUBLE_EQ(result.x, 4.0);
    EXPECT_DOUBLE_EQ(result.y, 6.0);
}

TEST(Vec2Test, SubtractionWorksCorrectly) {
    Vec2 a{5.0, 7.0};
    Vec2 b{2.0, 3.0};

    Vec2 result = a - b;

    EXPECT_DOUBLE_EQ(result.x, 3.0);
    EXPECT_DOUBLE_EQ(result.y, 4.0);
}

TEST(Vec2Test, ScalarMultiplicationWorksCorrectly) {
    Vec2 v{2.0, -3.0};

    Vec2 result = v * 2.0;

    EXPECT_DOUBLE_EQ(result.x, 4.0);
    EXPECT_DOUBLE_EQ(result.y, -6.0);
}

TEST(Vec2Test, NormalizationProducesUnitVector) {
    Vec2 v{3.0, 4.0};

    Vec2 result = v.normalized();

    EXPECT_NEAR(result.x, 0.6, 1e-9);
    EXPECT_NEAR(result.y, 0.8, 1e-9);
    EXPECT_NEAR(result.magnitude(), 1.0, 1e-9);
}