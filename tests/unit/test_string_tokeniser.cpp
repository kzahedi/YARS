#include <catch2/catch_test_macros.hpp>

#include <yars/util/StringTokeniser.h>
#include <string>
#include <vector>

using namespace yars;

TEST_CASE("StringTokeniser basic functionality", "[StringTokeniser]") {
    std::vector<std::string> result;

    SECTION("tokenize with comma delimiter") {
        result = StringTokeniser::tokenise("a,b,c", ",");
        REQUIRE(result.size() == 3);
        REQUIRE(result[0] == "a");
        REQUIRE(result[1] == "b");
        REQUIRE(result[2] == "c");
    }

    SECTION("tokenize with space delimiter") {
        result = StringTokeniser::tokenise("hello world test", " ");
        REQUIRE(result.size() == 3);
        REQUIRE(result[0] == "hello");
        REQUIRE(result[1] == "world");
        REQUIRE(result[2] == "test");
    }

    SECTION("tokenize with multiple delimiter characters") {
        // Note: delimiter string is treated as a SET of delimiter chars
        result = StringTokeniser::tokenise("a,b;c", ",;");
        REQUIRE(result.size() == 3);
        REQUIRE(result[0] == "a");
        REQUIRE(result[1] == "b");
        REQUIRE(result[2] == "c");
    }
}

TEST_CASE("StringTokeniser edge cases", "[StringTokeniser]") {
    std::vector<std::string> result;

    SECTION("empty string returns empty vector") {
        result = StringTokeniser::tokenise("", ",");
        REQUIRE(result.size() == 0);
    }

    SECTION("no delimiter found") {
        result = StringTokeniser::tokenise("hello", ",");
        REQUIRE(result.size() == 1);
        REQUIRE(result[0] == "hello");
    }

    SECTION("delimiter at start") {
        result = StringTokeniser::tokenise(",a,b", ",");
        REQUIRE(result.size() == 3);
        REQUIRE(result[0] == "");
        REQUIRE(result[1] == "a");
        REQUIRE(result[2] == "b");
    }

    SECTION("delimiter at end") {
        result = StringTokeniser::tokenise("a,b,", ",");
        REQUIRE(result.size() == 3);
        REQUIRE(result[0] == "a");
        REQUIRE(result[1] == "b");
        REQUIRE(result[2] == "");
    }
}
