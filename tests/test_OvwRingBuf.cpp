#include "pch.h"
#include <vector>
#include "..\debug_utils\OvwRingBuf.h"

TEST(OvwRingBuf, ctor)
{
	dbgutils::OvwRingBuf<int>	buf(3);

	EXPECT_EQ(buf.capacity(), 3);
	EXPECT_EQ(buf.size(), 0);
	EXPECT_TRUE(buf.empty());
	EXPECT_FALSE(buf.full());
}

TEST(OvwRingBuf, SizeOfOneAfterOnePush)
{
	dbgutils::OvwRingBuf<int>	buf(3);
	buf.push_back(1);

	EXPECT_EQ(buf.size(), 1);
}

TEST(OvwRingBuf, PushOneAndPeek)
{
	dbgutils::OvwRingBuf<int>	buf(3);
	buf.push_back(1);

	auto got = buf.peek(0);
	auto expected = 1;
	EXPECT_EQ(got, expected);
}

TEST(OvwRingBuf, PushPop)
{
	dbgutils::OvwRingBuf<int>	buf(3);
	buf.push_back(1);

	auto got = buf.pop_front();
	auto expected = 1;
	EXPECT_EQ(got, expected);

	EXPECT_EQ(buf.size(), 0);
}

// pop_collect pops and collects at most n items from the front of a buffer.
template <class T>
static std::vector<T> pop_collect(dbgutils::OvwRingBuf<T> &buf, size_t n)
{
	std::vector<T> items;

	for (size_t i = 0; !buf.empty() && i < n; i++) {
		items.push_back(buf.pop_front());
	}

	return items;
}

TEST(OvwRingBuf, FillAndPopAll)
{
	dbgutils::OvwRingBuf<int>	buf(3);
	for (size_t i = 0; i < buf.capacity(); i++) {
		buf.push_back(i);
	}

	EXPECT_TRUE(buf.full());
	EXPECT_EQ(buf.size(), buf.capacity());

	auto got = pop_collect(buf, buf.capacity());
	auto expected = std::vector<int>{ 0,1,2 };
	EXPECT_EQ(got, expected);

	EXPECT_TRUE(buf.empty());
}

// peek_collect peeks and collects at most n items from the front of a buffer.
// The items are not removed from the buffer.
template <class T>
static std::vector<T> peek_collect(dbgutils::OvwRingBuf<T> &buf, size_t n)
{
	std::vector<T> items;

	for (size_t i = 0; !buf.empty() && i < n; i++) {
		items.push_back(buf.peek(i));
	}

	return items;
}

TEST(OvwRingBuf, FillAndPeekAll)
{
	dbgutils::OvwRingBuf<int>	buf(3);
	for (size_t i = 0; i < buf.capacity(); i++) {
		buf.push_back(i);
	}

	EXPECT_TRUE(buf.full());
	EXPECT_EQ(buf.size(), buf.capacity());

	auto got = peek_collect(buf, buf.capacity());
	auto expected = std::vector<int>{ 0,1,2 };
	EXPECT_EQ(got, expected);
}
