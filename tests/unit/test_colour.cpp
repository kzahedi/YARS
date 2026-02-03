#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include <yars/types/Colour.h>

using Catch::Approx;

TEST_CASE("Colour construction", "[Colour]") {
    SECTION("default constructor creates black with alpha 1") {
        Colour c;
        REQUIRE(c.red() == Approx(0.0));
        REQUIRE(c.green() == Approx(0.0));
        REQUIRE(c.blue() == Approx(0.0));
        REQUIRE(c.alpha() == Approx(1.0));
    }

    SECTION("constructor with RGBA") {
        Colour c(0.5, 0.6, 0.7, 0.8);
        REQUIRE(c.red() == Approx(0.5));
        REQUIRE(c.green() == Approx(0.6));
        REQUIRE(c.blue() == Approx(0.7));
        REQUIRE(c.alpha() == Approx(0.8));
    }
}

TEST_CASE("Colour setters", "[Colour]") {
    Colour c;

    c.setRed(0.1);
    c.setGreen(0.2);
    c.setBlue(0.3);
    c.setAlpha(0.4);

    REQUIRE(c.red() == Approx(0.1));
    REQUIRE(c.green() == Approx(0.2));
    REQUIRE(c.blue() == Approx(0.3));
    REQUIRE(c.alpha() == Approx(0.4));
}

TEST_CASE("Colour copy assignment", "[Colour]") {
    Colour c1(0.5, 0.6, 0.7, 0.8);
    Colour c2;
    c2 = c1;

    REQUIRE(c2.red() == Approx(0.5));
    REQUIRE(c2.green() == Approx(0.6));
    REQUIRE(c2.blue() == Approx(0.7));
    REQUIRE(c2.alpha() == Approx(0.8));
}

TEST_CASE("Colour inverse", "[Colour]") {
    Colour c(0.2, 0.3, 0.4, 0.5);
    c.inverse();

    REQUIRE(c.red() == Approx(0.8));
    REQUIRE(c.green() == Approx(0.7));
    REQUIRE(c.blue() == Approx(0.6));
    REQUIRE(c.alpha() == Approx(0.5));  // alpha unchanged
}

TEST_CASE("Colour predefined colors", "[Colour]") {
    SECTION("BLACK") {
        REQUIRE(Colour::BLACK.red() == Approx(0.0));
        REQUIRE(Colour::BLACK.green() == Approx(0.0));
        REQUIRE(Colour::BLACK.blue() == Approx(0.0));
        REQUIRE(Colour::BLACK.alpha() == Approx(1.0));
    }

    SECTION("RED") {
        REQUIRE(Colour::RED.red() == Approx(1.0));
        REQUIRE(Colour::RED.green() == Approx(0.0));
        REQUIRE(Colour::RED.blue() == Approx(0.0));
        REQUIRE(Colour::RED.alpha() == Approx(1.0));
    }

    SECTION("BLUE") {
        REQUIRE(Colour::BLUE.red() == Approx(0.0));
        REQUIRE(Colour::BLUE.green() == Approx(0.0));
        REQUIRE(Colour::BLUE.blue() == Approx(1.0));
        REQUIRE(Colour::BLUE.alpha() == Approx(1.0));
    }

    SECTION("WHITE") {
        REQUIRE(Colour::WHITE.red() == Approx(1.0));
        REQUIRE(Colour::WHITE.green() == Approx(1.0));
        REQUIRE(Colour::WHITE.blue() == Approx(1.0));
        REQUIRE(Colour::WHITE.alpha() == Approx(1.0));
    }
}
