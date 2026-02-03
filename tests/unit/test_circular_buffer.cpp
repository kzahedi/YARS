#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include <yars/util/CircularBuffer.h>
#include <string>

TEST_CASE("CircularBuffer basic operations", "[CircularBuffer]") {
    CircularBuffer<int> buffer;

    SECTION("default constructor creates empty buffer with zero capacity") {
        REQUIRE(buffer.size() == 0);
        REQUIRE(buffer.capacity() == 0);
        REQUIRE(buffer.empty());
    }

    SECTION("set_capacity changes capacity") {
        buffer.set_capacity(5);
        REQUIRE(buffer.capacity() == 5);
        REQUIRE(buffer.size() == 0);
        REQUIRE(buffer.empty());
    }
}

TEST_CASE("CircularBuffer with initial capacity", "[CircularBuffer]") {
    CircularBuffer<int> buffer(5);

    REQUIRE(buffer.capacity() == 5);
    REQUIRE(buffer.size() == 0);
    REQUIRE(buffer.empty());
    REQUIRE_FALSE(buffer.full());
}

TEST_CASE("CircularBuffer push_back", "[CircularBuffer]") {
    CircularBuffer<int> buffer(3);

    SECTION("push_back adds elements") {
        buffer.push_back(1);
        REQUIRE(buffer.size() == 1);
        REQUIRE(buffer[0] == 1);

        buffer.push_back(2);
        REQUIRE(buffer.size() == 2);
        REQUIRE(buffer[0] == 1);
        REQUIRE(buffer[1] == 2);

        buffer.push_back(3);
        REQUIRE(buffer.size() == 3);
        REQUIRE(buffer.full());
    }

    SECTION("push_back wraps around when full") {
        buffer.push_back(1);
        buffer.push_back(2);
        buffer.push_back(3);
        REQUIRE(buffer.size() == 3);

        // Adding 4th element should remove first
        buffer.push_back(4);
        REQUIRE(buffer.size() == 3);
        REQUIRE(buffer[0] == 2);
        REQUIRE(buffer[1] == 3);
        REQUIRE(buffer[2] == 4);

        // Adding 5th element should remove second
        buffer.push_back(5);
        REQUIRE(buffer.size() == 3);
        REQUIRE(buffer[0] == 3);
        REQUIRE(buffer[1] == 4);
        REQUIRE(buffer[2] == 5);
    }
}

TEST_CASE("CircularBuffer front and back", "[CircularBuffer]") {
    CircularBuffer<int> buffer(5);

    buffer.push_back(10);
    buffer.push_back(20);
    buffer.push_back(30);

    REQUIRE(buffer.front() == 10);
    REQUIRE(buffer.back() == 30);
}

TEST_CASE("CircularBuffer clear", "[CircularBuffer]") {
    CircularBuffer<int> buffer(5);

    buffer.push_back(1);
    buffer.push_back(2);
    buffer.push_back(3);
    REQUIRE(buffer.size() == 3);

    buffer.clear();
    REQUIRE(buffer.size() == 0);
    REQUIRE(buffer.empty());
    REQUIRE(buffer.capacity() == 5);  // capacity unchanged
}

TEST_CASE("CircularBuffer iteration", "[CircularBuffer]") {
    CircularBuffer<int> buffer(5);

    buffer.push_back(1);
    buffer.push_back(2);
    buffer.push_back(3);

    SECTION("range-based for loop") {
        int sum = 0;
        for (const auto& val : buffer) {
            sum += val;
        }
        REQUIRE(sum == 6);
    }

    SECTION("iterator begin/end") {
        auto it = buffer.begin();
        REQUIRE(*it == 1);
        ++it;
        REQUIRE(*it == 2);
        ++it;
        REQUIRE(*it == 3);
        ++it;
        REQUIRE(it == buffer.end());
    }
}

TEST_CASE("CircularBuffer with strings", "[CircularBuffer]") {
    CircularBuffer<std::string> buffer(3);

    buffer.push_back("hello");
    buffer.push_back("world");
    buffer.push_back("!");

    REQUIRE(buffer.size() == 3);
    REQUIRE(buffer[0] == "hello");
    REQUIRE(buffer[1] == "world");
    REQUIRE(buffer[2] == "!");

    buffer.push_back("test");
    REQUIRE(buffer[0] == "world");
    REQUIRE(buffer[1] == "!");
    REQUIRE(buffer[2] == "test");
}

TEST_CASE("CircularBuffer set_capacity shrinks", "[CircularBuffer]") {
    CircularBuffer<int> buffer(5);

    buffer.push_back(1);
    buffer.push_back(2);
    buffer.push_back(3);
    buffer.push_back(4);
    buffer.push_back(5);
    REQUIRE(buffer.size() == 5);

    // Shrink capacity - should remove oldest elements
    buffer.set_capacity(3);
    REQUIRE(buffer.capacity() == 3);
    REQUIRE(buffer.size() == 3);
    // Oldest elements (1, 2) should be removed
    REQUIRE(buffer[0] == 3);
    REQUIRE(buffer[1] == 4);
    REQUIRE(buffer[2] == 5);
}

TEST_CASE("CircularBuffer with zero capacity ignores push_back", "[CircularBuffer]") {
    CircularBuffer<int> buffer;  // capacity = 0

    buffer.push_back(1);
    REQUIRE(buffer.size() == 0);
    REQUIRE(buffer.empty());
}
