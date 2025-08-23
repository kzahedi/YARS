#include <gtest/gtest.h>
#include <yars/types/Pose.h>

// Note: Using yars namespace where available
using yars::Pose;

class PoseTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up test fixtures
        pose1 = Pose(1.0, 2.0, 3.0, 0.0, 0.0, 0.0);
        pose2 = Pose(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    }

    Pose pose1, pose2;
};

TEST_F(PoseTest, DefaultConstructor) {
    Pose p;
    EXPECT_DOUBLE_EQ(p.position.x, 0.0);
    EXPECT_DOUBLE_EQ(p.position.y, 0.0);
    EXPECT_DOUBLE_EQ(p.position.z, 0.0);
}

TEST_F(PoseTest, EulerConstructor) {
    EXPECT_DOUBLE_EQ(pose1.position.x, 1.0);
    EXPECT_DOUBLE_EQ(pose1.position.y, 2.0);
    EXPECT_DOUBLE_EQ(pose1.position.z, 3.0);
}

TEST_F(PoseTest, QuaternionConstructor) {
    EXPECT_DOUBLE_EQ(pose2.position.x, 0.0);
    EXPECT_DOUBLE_EQ(pose2.position.y, 0.0);
    EXPECT_DOUBLE_EQ(pose2.position.z, 0.0);
    EXPECT_DOUBLE_EQ(pose2.q.w, 1.0);
}

TEST_F(PoseTest, CopyConstructor) {
    Pose p(pose1);
    EXPECT_DOUBLE_EQ(p.position.x, pose1.position.x);
    EXPECT_DOUBLE_EQ(p.position.y, pose1.position.y);
    EXPECT_DOUBLE_EQ(p.position.z, pose1.position.z);
}

TEST_F(PoseTest, AssignmentOperator) {
    Pose p;
    p = pose1;
    EXPECT_DOUBLE_EQ(p.position.x, pose1.position.x);
    EXPECT_DOUBLE_EQ(p.position.y, pose1.position.y);
    EXPECT_DOUBLE_EQ(p.position.z, pose1.position.z);
}

TEST_F(PoseTest, AngleConversion) {
    Pose p(0.0, 0.0, 0.0, 0.0, 0.0, M_PI/2.0);
    P3D degrees = p.deg();
    EXPECT_NEAR(degrees.z, 90.0, 1e-10);
}