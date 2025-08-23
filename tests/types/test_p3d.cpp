#include <gtest/gtest.h>
#include <yars/types/P3D.h>

// Note: P3D not yet in namespace yars

class P3DTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up test fixtures
        p1 = P3D(1.0, 2.0, 3.0);
        p2 = P3D(4.0, 5.0, 6.0);
        zero = P3D(0.0, 0.0, 0.0);
    }

    P3D p1, p2, zero;
};

TEST_F(P3DTest, DefaultConstructor) {
    P3D p;
    EXPECT_DOUBLE_EQ(p.x, 0.0);
    EXPECT_DOUBLE_EQ(p.y, 0.0);
    EXPECT_DOUBLE_EQ(p.z, 0.0);
}

TEST_F(P3DTest, ParameterizedConstructor) {
    EXPECT_DOUBLE_EQ(p1.x, 1.0);
    EXPECT_DOUBLE_EQ(p1.y, 2.0);
    EXPECT_DOUBLE_EQ(p1.z, 3.0);
}

TEST_F(P3DTest, CopyConstructor) {
    P3D p3(p1);
    EXPECT_DOUBLE_EQ(p3.x, p1.x);
    EXPECT_DOUBLE_EQ(p3.y, p1.y);
    EXPECT_DOUBLE_EQ(p3.z, p1.z);
}

TEST_F(P3DTest, AdditionOperator) {
    P3D result = p1 + p2;
    EXPECT_DOUBLE_EQ(result.x, 5.0);
    EXPECT_DOUBLE_EQ(result.y, 7.0);
    EXPECT_DOUBLE_EQ(result.z, 9.0);
}

TEST_F(P3DTest, SubtractionOperator) {
    P3D result = p2 - p1;
    EXPECT_DOUBLE_EQ(result.x, 3.0);
    EXPECT_DOUBLE_EQ(result.y, 3.0);
    EXPECT_DOUBLE_EQ(result.z, 3.0);
}

TEST_F(P3DTest, ScalarMultiplication) {
    P3D result = p1 * 2.0;
    EXPECT_DOUBLE_EQ(result.x, 2.0);
    EXPECT_DOUBLE_EQ(result.y, 4.0);
    EXPECT_DOUBLE_EQ(result.z, 6.0);
}

TEST_F(P3DTest, EqualityOperator) {
    P3D p3(1.0, 2.0, 3.0);
    EXPECT_TRUE(p1 == p3);
    EXPECT_FALSE(p1 == p2);
}

TEST_F(P3DTest, Length) {
    P3D unit(1.0, 0.0, 0.0);
    EXPECT_DOUBLE_EQ(unit.length(), 1.0);
    
    P3D p(3.0, 4.0, 0.0);
    EXPECT_DOUBLE_EQ(p.length(), 5.0);
}