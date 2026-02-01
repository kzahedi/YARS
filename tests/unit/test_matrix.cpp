#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <yars/types/Matrix.h>

using Catch::Matchers::WithinAbs;

TEST_CASE("Matrix default constructor", "[Matrix]") {
  Matrix m;
  REQUIRE(m.rows() == 0);
  REQUIRE(m.cols() == 0);
}

TEST_CASE("Matrix size constructor", "[Matrix]") {
  Matrix m(3, 4);
  REQUIRE(m.rows() == 3);
  REQUIRE(m.cols() == 4);
}

TEST_CASE("Matrix initialized to value", "[Matrix]") {
  Matrix m(2, 2, 5.0);
  REQUIRE(m(0, 0) == 5.0);
  REQUIRE(m(0, 1) == 5.0);
  REQUIRE(m(1, 0) == 5.0);
  REQUIRE(m(1, 1) == 5.0);
}

TEST_CASE("Matrix element access", "[Matrix]") {
  Matrix m(2, 2);
  m(0, 0) = 1.0;
  m(0, 1) = 2.0;
  m(1, 0) = 3.0;
  m(1, 1) = 4.0;
  REQUIRE(m(0, 0) == 1.0);
  REQUIRE(m(0, 1) == 2.0);
  REQUIRE(m(1, 0) == 3.0);
  REQUIRE(m(1, 1) == 4.0);
}

TEST_CASE("Matrix copy constructor", "[Matrix]") {
  Matrix a(2, 2);
  a(0, 0) = 1.0;
  a(1, 1) = 4.0;
  Matrix b(a);
  REQUIRE(b(0, 0) == 1.0);
  REQUIRE(b(1, 1) == 4.0);
}

TEST_CASE("Matrix assignment", "[Matrix]") {
  Matrix a(2, 2);
  a(0, 0) = 1.0;
  a(1, 1) = 4.0;
  Matrix b;
  b = a;
  REQUIRE(b(0, 0) == 1.0);
  REQUIRE(b(1, 1) == 4.0);
}

TEST_CASE("Matrix addition", "[Matrix]") {
  Matrix a(2, 2, 1.0);
  Matrix b(2, 2, 2.0);
  Matrix c = a + b;
  REQUIRE(c(0, 0) == 3.0);
  REQUIRE(c(1, 1) == 3.0);
}

TEST_CASE("Matrix subtraction", "[Matrix]") {
  Matrix a(2, 2, 5.0);
  Matrix b(2, 2, 2.0);
  Matrix c = a - b;
  REQUIRE(c(0, 0) == 3.0);
  REQUIRE(c(1, 1) == 3.0);
}

TEST_CASE("Matrix scalar multiplication", "[Matrix]") {
  Matrix a(2, 2, 2.0);
  Matrix b = a * 3.0;
  REQUIRE(b(0, 0) == 6.0);
  REQUIRE(b(1, 1) == 6.0);
}

TEST_CASE("Matrix multiplication", "[Matrix]") {
  Matrix a(2, 2);
  a(0, 0) = 1.0; a(0, 1) = 2.0;
  a(1, 0) = 3.0; a(1, 1) = 4.0;

  Matrix b(2, 2);
  b(0, 0) = 5.0; b(0, 1) = 6.0;
  b(1, 0) = 7.0; b(1, 1) = 8.0;

  Matrix c = a * b;
  // c[0,0] = 1*5 + 2*7 = 19
  // c[0,1] = 1*6 + 2*8 = 22
  // c[1,0] = 3*5 + 4*7 = 43
  // c[1,1] = 3*6 + 4*8 = 50
  REQUIRE(c(0, 0) == 19.0);
  REQUIRE(c(0, 1) == 22.0);
  REQUIRE(c(1, 0) == 43.0);
  REQUIRE(c(1, 1) == 50.0);
}

TEST_CASE("Matrix diagonal", "[Matrix]") {
  Matrix m(3, 3);
  m.setDiagonalMatrix(1.0);
  REQUIRE(m(0, 0) == 1.0);
  REQUIRE(m(1, 1) == 1.0);
  REQUIRE(m(2, 2) == 1.0);
  REQUIRE(m(0, 1) == 0.0);
  REQUIRE(m(1, 0) == 0.0);
}

TEST_CASE("Matrix row sum", "[Matrix]") {
  Matrix m(2, 3);
  m(0, 0) = 1.0; m(0, 1) = 2.0; m(0, 2) = 3.0;
  m(1, 0) = 4.0; m(1, 1) = 5.0; m(1, 2) = 6.0;
  REQUIRE(m.rowSum(0) == 6.0);
  REQUIRE(m.rowSum(1) == 15.0);
}

TEST_CASE("Matrix column sum", "[Matrix]") {
  Matrix m(2, 3);
  m(0, 0) = 1.0; m(0, 1) = 2.0; m(0, 2) = 3.0;
  m(1, 0) = 4.0; m(1, 1) = 5.0; m(1, 2) = 6.0;
  REQUIRE(m.colSum(0) == 5.0);
  REQUIRE(m.colSum(1) == 7.0);
  REQUIRE(m.colSum(2) == 9.0);
}

TEST_CASE("Matrix 2x2 determinant", "[Matrix]") {
  Matrix m(2, 2);
  m(0, 0) = 1.0; m(0, 1) = 2.0;
  m(1, 0) = 3.0; m(1, 1) = 4.0;
  // det = 1*4 - 2*3 = -2
  REQUIRE_THAT(m.det(), WithinAbs(-2.0, 1e-10));
}

TEST_CASE("Matrix transpose", "[Matrix]") {
  Matrix m(2, 3);
  m(0, 0) = 1.0; m(0, 1) = 2.0; m(0, 2) = 3.0;
  m(1, 0) = 4.0; m(1, 1) = 5.0; m(1, 2) = 6.0;
  m.transpose();
  REQUIRE(m.rows() == 3);
  REQUIRE(m.cols() == 2);
  REQUIRE(m(0, 0) == 1.0);
  REQUIRE(m(0, 1) == 4.0);
  REQUIRE(m(1, 0) == 2.0);
  REQUIRE(m(1, 1) == 5.0);
  REQUIRE(m(2, 0) == 3.0);
  REQUIRE(m(2, 1) == 6.0);
}

TEST_CASE("Matrix L2 norm", "[Matrix]") {
  Matrix m(2, 2);
  m(0, 0) = 1.0; m(0, 1) = 2.0;
  m(1, 0) = 3.0; m(1, 1) = 4.0;
  // L2 = sqrt(1 + 4 + 9 + 16) = sqrt(30)
  REQUIRE_THAT(m.L2(), WithinAbs(sqrt(30.0), 1e-10));
}
