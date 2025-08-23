#include <gtest/gtest.h>

// Main test runner - Google Test will handle everything
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}