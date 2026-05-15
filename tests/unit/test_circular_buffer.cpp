#include <gtest/gtest.h>
#include <yars/util/CircularBuffer.h>
#include <string>

TEST(CircularBuffer, DefaultConstructorCreatesEmptyBufferWithZeroCapacity)
{
  CircularBuffer<int> buffer;
  EXPECT_EQ(buffer.size(), 0u);
  EXPECT_EQ(buffer.capacity(), 0u);
  EXPECT_TRUE(buffer.empty());
}

TEST(CircularBuffer, SetCapacityChangesCapacity)
{
  CircularBuffer<int> buffer;
  buffer.set_capacity(5);
  EXPECT_EQ(buffer.capacity(), 5u);
  EXPECT_EQ(buffer.size(), 0u);
  EXPECT_TRUE(buffer.empty());
}

TEST(CircularBuffer, ConstructorWithInitialCapacity)
{
  CircularBuffer<int> buffer(5);
  EXPECT_EQ(buffer.capacity(), 5u);
  EXPECT_EQ(buffer.size(), 0u);
  EXPECT_TRUE(buffer.empty());
  EXPECT_FALSE(buffer.full());
}

TEST(CircularBuffer, PushBackAddsElements)
{
  CircularBuffer<int> buffer(3);

  buffer.push_back(1);
  EXPECT_EQ(buffer.size(), 1u);
  EXPECT_EQ(buffer[0], 1);

  buffer.push_back(2);
  EXPECT_EQ(buffer.size(), 2u);
  EXPECT_EQ(buffer[0], 1);
  EXPECT_EQ(buffer[1], 2);

  buffer.push_back(3);
  EXPECT_EQ(buffer.size(), 3u);
  EXPECT_TRUE(buffer.full());
}

TEST(CircularBuffer, PushBackWrapsAroundWhenFull)
{
  CircularBuffer<int> buffer(3);
  buffer.push_back(1);
  buffer.push_back(2);
  buffer.push_back(3);
  EXPECT_EQ(buffer.size(), 3u);

  buffer.push_back(4);
  EXPECT_EQ(buffer.size(), 3u);
  EXPECT_EQ(buffer[0], 2);
  EXPECT_EQ(buffer[1], 3);
  EXPECT_EQ(buffer[2], 4);

  buffer.push_back(5);
  EXPECT_EQ(buffer.size(), 3u);
  EXPECT_EQ(buffer[0], 3);
  EXPECT_EQ(buffer[1], 4);
  EXPECT_EQ(buffer[2], 5);
}

TEST(CircularBuffer, FrontAndBack)
{
  CircularBuffer<int> buffer(5);
  buffer.push_back(10);
  buffer.push_back(20);
  buffer.push_back(30);

  EXPECT_EQ(buffer.front(), 10);
  EXPECT_EQ(buffer.back(), 30);
}

TEST(CircularBuffer, ClearResetsSizeButKeepsCapacity)
{
  CircularBuffer<int> buffer(5);
  buffer.push_back(1);
  buffer.push_back(2);
  buffer.push_back(3);
  EXPECT_EQ(buffer.size(), 3u);

  buffer.clear();
  EXPECT_EQ(buffer.size(), 0u);
  EXPECT_TRUE(buffer.empty());
  EXPECT_EQ(buffer.capacity(), 5u);
}

TEST(CircularBuffer, RangeBasedForLoop)
{
  CircularBuffer<int> buffer(5);
  buffer.push_back(1);
  buffer.push_back(2);
  buffer.push_back(3);

  int sum = 0;
  for (const auto& val : buffer) sum += val;
  EXPECT_EQ(sum, 6);
}

TEST(CircularBuffer, IteratorBeginEnd)
{
  CircularBuffer<int> buffer(5);
  buffer.push_back(1);
  buffer.push_back(2);
  buffer.push_back(3);

  auto it = buffer.begin();
  EXPECT_EQ(*it, 1);
  ++it;
  EXPECT_EQ(*it, 2);
  ++it;
  EXPECT_EQ(*it, 3);
  ++it;
  EXPECT_TRUE(it == buffer.end());
}

TEST(CircularBuffer, WithStrings)
{
  CircularBuffer<std::string> buffer(3);
  buffer.push_back("hello");
  buffer.push_back("world");
  buffer.push_back("!");

  EXPECT_EQ(buffer.size(), 3u);
  EXPECT_EQ(buffer[0], "hello");
  EXPECT_EQ(buffer[1], "world");
  EXPECT_EQ(buffer[2], "!");

  buffer.push_back("test");
  EXPECT_EQ(buffer[0], "world");
  EXPECT_EQ(buffer[1], "!");
  EXPECT_EQ(buffer[2], "test");
}

TEST(CircularBuffer, SetCapacityShrinksAndDropsOldest)
{
  CircularBuffer<int> buffer(5);
  for (int i = 1; i <= 5; ++i) buffer.push_back(i);
  EXPECT_EQ(buffer.size(), 5u);

  buffer.set_capacity(3);
  EXPECT_EQ(buffer.capacity(), 3u);
  EXPECT_EQ(buffer.size(), 3u);
  EXPECT_EQ(buffer[0], 3);
  EXPECT_EQ(buffer[1], 4);
  EXPECT_EQ(buffer[2], 5);
}

TEST(CircularBuffer, ZeroCapacityIgnoresPushBack)
{
  CircularBuffer<int> buffer;
  buffer.push_back(1);
  EXPECT_EQ(buffer.size(), 0u);
  EXPECT_TRUE(buffer.empty());
}
