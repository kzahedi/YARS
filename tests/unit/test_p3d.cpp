#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <yars/types/P3D.h>

using Catch::Matchers::WithinAbs;

TEST_CASE("P3D default constructor", "[P3D]") {
  P3D p;
  REQUIRE(p.x == 0.0);
  REQUIRE(p.y == 0.0);
  REQUIRE(p.z == 0.0);
}

TEST_CASE("P3D value constructor", "[P3D]") {
  P3D p(1.0, 2.0, 3.0);
  REQUIRE(p.x == 1.0);
  REQUIRE(p.y == 2.0);
  REQUIRE(p.z == 3.0);
}

TEST_CASE("P3D copy constructor", "[P3D]") {
  P3D a(1.0, 2.0, 3.0);
  P3D b(a);
  REQUIRE(b.x == 1.0);
  REQUIRE(b.y == 2.0);
  REQUIRE(b.z == 3.0);
}

TEST_CASE("P3D array constructor", "[P3D]") {
  double arr[3] = {1.0, 2.0, 3.0};
  P3D p(arr);
  REQUIRE(p.x == 1.0);
  REQUIRE(p.y == 2.0);
  REQUIRE(p.z == 3.0);
}

TEST_CASE("P3D assignment operator", "[P3D]") {
  P3D a(1.0, 2.0, 3.0);
  P3D b;
  b = a;
  REQUIRE(b.x == 1.0);
  REQUIRE(b.y == 2.0);
  REQUIRE(b.z == 3.0);
}

TEST_CASE("P3D scalar assignment", "[P3D]") {
  P3D p;
  p = 5.0;
  REQUIRE(p.x == 5.0);
  REQUIRE(p.y == 5.0);
  REQUIRE(p.z == 5.0);
}

TEST_CASE("P3D addition", "[P3D]") {
  P3D a(1.0, 2.0, 3.0);
  P3D b(4.0, 5.0, 6.0);
  P3D c = a + b;
  REQUIRE(c.x == 5.0);
  REQUIRE(c.y == 7.0);
  REQUIRE(c.z == 9.0);
}

TEST_CASE("P3D subtraction", "[P3D]") {
  P3D a(4.0, 5.0, 6.0);
  P3D b(1.0, 2.0, 3.0);
  P3D c = a - b;
  REQUIRE(c.x == 3.0);
  REQUIRE(c.y == 3.0);
  REQUIRE(c.z == 3.0);
}

TEST_CASE("P3D scalar multiplication", "[P3D]") {
  P3D a(1.0, 2.0, 3.0);
  P3D b = a * 2.0;
  REQUIRE(b.x == 2.0);
  REQUIRE(b.y == 4.0);
  REQUIRE(b.z == 6.0);
}

TEST_CASE("P3D in-place addition", "[P3D]") {
  P3D a(1.0, 2.0, 3.0);
  P3D b(4.0, 5.0, 6.0);
  a += b;
  REQUIRE(a.x == 5.0);
  REQUIRE(a.y == 7.0);
  REQUIRE(a.z == 9.0);
}

TEST_CASE("P3D in-place multiplication", "[P3D]") {
  P3D a(1.0, 2.0, 3.0);
  a *= 2.0;
  REQUIRE(a.x == 2.0);
  REQUIRE(a.y == 4.0);
  REQUIRE(a.z == 6.0);
}

TEST_CASE("P3D in-place division", "[P3D]") {
  P3D a(2.0, 4.0, 6.0);
  a /= 2.0;
  REQUIRE(a.x == 1.0);
  REQUIRE(a.y == 2.0);
  REQUIRE(a.z == 3.0);
}

TEST_CASE("P3D length", "[P3D]") {
  P3D a(3.0, 4.0, 0.0);
  REQUIRE_THAT(a.length(), WithinAbs(5.0, 1e-10));
}

TEST_CASE("P3D normalize", "[P3D]") {
  P3D a(3.0, 0.0, 4.0);
  a.normalise();
  REQUIRE_THAT(a.length(), WithinAbs(1.0, 1e-10));
  REQUIRE_THAT(a.x, WithinAbs(0.6, 1e-10));
  REQUIRE_THAT(a.z, WithinAbs(0.8, 1e-10));
}

TEST_CASE("P3D dot product", "[P3D]") {
  P3D a(1.0, 2.0, 3.0);
  P3D b(4.0, 5.0, 6.0);
  double dot = a.dot(b);
  REQUIRE(dot == 32.0); // 1*4 + 2*5 + 3*6 = 4 + 10 + 18
}

TEST_CASE("P3D cross product", "[P3D]") {
  P3D a(1.0, 0.0, 0.0);
  P3D b(0.0, 1.0, 0.0);
  P3D c = a * b;
  REQUIRE_THAT(c.x, WithinAbs(0.0, 1e-10));
  REQUIRE_THAT(c.y, WithinAbs(0.0, 1e-10));
  REQUIRE_THAT(c.z, WithinAbs(1.0, 1e-10));
}

TEST_CASE("P3D equality", "[P3D]") {
  P3D a(1.0, 2.0, 3.0);
  P3D b(1.0, 2.0, 3.0);
  P3D c(1.0, 2.0, 4.0);
  REQUIRE(a == b);
  REQUIRE(a != c);
}

TEST_CASE("P3D distance", "[P3D]") {
  P3D a(0.0, 0.0, 0.0);
  P3D b(3.0, 4.0, 0.0);
  REQUIRE_THAT(a.dist(b), WithinAbs(5.0, 1e-10));
}
