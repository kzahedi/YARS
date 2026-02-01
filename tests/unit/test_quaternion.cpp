#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <yars/types/Quaternion.h>
#include <cmath>

using Catch::Matchers::WithinAbs;

TEST_CASE("Quaternion default constructor", "[Quaternion]") {
  Quaternion q;
  REQUIRE(q.w == 1.0);
  REQUIRE(q.x == 0.0);
  REQUIRE(q.y == 0.0);
  REQUIRE(q.z == 0.0);
}

TEST_CASE("Quaternion value constructor", "[Quaternion]") {
  Quaternion q(1.0, 2.0, 3.0, 4.0);
  REQUIRE(q.w == 1.0);
  REQUIRE(q.x == 2.0);
  REQUIRE(q.y == 3.0);
  REQUIRE(q.z == 4.0);
}

TEST_CASE("Quaternion copy constructor", "[Quaternion]") {
  Quaternion a(1.0, 2.0, 3.0, 4.0);
  Quaternion b(a);
  REQUIRE(b.w == 1.0);
  REQUIRE(b.x == 2.0);
  REQUIRE(b.y == 3.0);
  REQUIRE(b.z == 4.0);
}

TEST_CASE("Quaternion assignment", "[Quaternion]") {
  Quaternion a(1.0, 2.0, 3.0, 4.0);
  Quaternion b;
  b = a;
  REQUIRE(b.w == 1.0);
  REQUIRE(b.x == 2.0);
  REQUIRE(b.y == 3.0);
  REQUIRE(b.z == 4.0);
}

TEST_CASE("Quaternion identity multiplication", "[Quaternion]") {
  Quaternion identity; // (1, 0, 0, 0)
  Quaternion q(0.5, 0.5, 0.5, 0.5);
  Quaternion result = identity * q;
  REQUIRE_THAT(result.w, WithinAbs(0.5, 1e-10));
  REQUIRE_THAT(result.x, WithinAbs(0.5, 1e-10));
  REQUIRE_THAT(result.y, WithinAbs(0.5, 1e-10));
  REQUIRE_THAT(result.z, WithinAbs(0.5, 1e-10));
}

TEST_CASE("Quaternion conjugate", "[Quaternion]") {
  Quaternion q(1.0, 2.0, 3.0, 4.0);
  q.conjugate();
  REQUIRE(q.w == 1.0);
  REQUIRE(q.x == -2.0);
  REQUIRE(q.y == -3.0);
  REQUIRE(q.z == -4.0);
}

TEST_CASE("Quaternion negation", "[Quaternion]") {
  Quaternion q(1.0, 2.0, 3.0, 4.0);
  Quaternion neg = -q;
  REQUIRE(neg.w == -1.0);
  REQUIRE(neg.x == -2.0);
  REQUIRE(neg.y == -3.0);
  REQUIRE(neg.z == -4.0);
}

TEST_CASE("Quaternion from euler angles", "[Quaternion]") {
  // Test identity rotation (no rotation)
  P3D euler(0.0, 0.0, 0.0);
  Quaternion q(euler);
  REQUIRE_THAT(q.w, WithinAbs(1.0, 1e-10));
  REQUIRE_THAT(q.x, WithinAbs(0.0, 1e-10));
  REQUIRE_THAT(q.y, WithinAbs(0.0, 1e-10));
  REQUIRE_THAT(q.z, WithinAbs(0.0, 1e-10));
}

TEST_CASE("Quaternion rotates vector", "[Quaternion]") {
  // 90 degree rotation around Z axis
  double angle = M_PI / 2.0;
  Quaternion q(cos(angle / 2.0), 0, 0, sin(angle / 2.0));

  P3D v(1.0, 0.0, 0.0);
  v *= q;

  REQUIRE_THAT(v.x, WithinAbs(0.0, 1e-10));
  REQUIRE_THAT(v.y, WithinAbs(1.0, 1e-10));
  REQUIRE_THAT(v.z, WithinAbs(0.0, 1e-10));
}

TEST_CASE("Quaternion to euler roundtrip", "[Quaternion]") {
  // Create quaternion from euler angles
  P3D originalEuler(0.1, 0.2, 0.3);
  Quaternion q;
  q << originalEuler;

  // Convert back to euler
  P3D recoveredEuler;
  recoveredEuler << q;

  REQUIRE_THAT(recoveredEuler.x, WithinAbs(originalEuler.x, 1e-6));
  REQUIRE_THAT(recoveredEuler.y, WithinAbs(originalEuler.y, 1e-6));
  REQUIRE_THAT(recoveredEuler.z, WithinAbs(originalEuler.z, 1e-6));
}
