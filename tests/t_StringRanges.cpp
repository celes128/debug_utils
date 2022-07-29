#include "pch.h"
#include "../debug_utils/EditBox.h"

TEST(StringRanges, EmptyString)
{
	auto s = L"";

	auto ranges = dbgutils::ComputeStringRanges(s);

	auto got = ranges.size();
	auto expected = 0;
	EXPECT_EQ(got, expected);
}

TEST(StringRanges, StringWithOneLetter)
{
	auto s = L"a";

	auto got = dbgutils::ComputeStringRanges(s);

	std::vector<dbgutils::StringRange> expected = {
		dbgutils::StringRange::MakeWordRange(0, 1)
	};

	EXPECT_EQ(got, expected);
}

TEST(StringRanges, StringWithTwoLetters)
{
	auto s = L"ab";

	auto got = dbgutils::ComputeStringRanges(s);

	std::vector<dbgutils::StringRange> expected = {
		dbgutils::StringRange::MakeWordRange(0, 2)
	};

	EXPECT_EQ(got, expected);
}

TEST(StringRanges, StringOfWhitespaces)
{
	auto s = L"     ";

	auto got = dbgutils::ComputeStringRanges(s);

	std::vector<dbgutils::StringRange> expected = {
		dbgutils::StringRange::MakeSpaceRange(0, 5)
	};

	EXPECT_EQ(got, expected);
}

TEST(StringRanges, String1)
{
	using StrRange = dbgutils::StringRange;

	auto s = L" ab c de ";

	auto got = dbgutils::ComputeStringRanges(s);

	std::vector<StrRange> expected = {
		StrRange::MakeSpaceRange(0, 1),
		StrRange::MakeWordRange(1, 2),
		StrRange::MakeSpaceRange(3, 1),
		StrRange::MakeWordRange(4, 1),
		StrRange::MakeSpaceRange(5, 1),
		StrRange::MakeWordRange(6, 2),
		StrRange::MakeSpaceRange(8, 1)
	};

	EXPECT_EQ(got, expected);
}