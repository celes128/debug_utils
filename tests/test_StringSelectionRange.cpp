#include "pch.h"
#include "../debug_utils/StringSelectionRange.h"

static void RunDragCaretTest(
	dbgutils::StringSelectionRange &ssr,
	dbgutils::Direction dragDirection, size_t dragAmount, size_t maxLength,
	const dbgutils::StringSelectionRange &expected)
{
	using Dir = dbgutils::Direction;
	using StrSelRange = dbgutils::StringSelectionRange;

	ssr.drag_caret(dragDirection, dragAmount, maxLength);

	auto got = StrSelRange(ssr.get_caret_side(), ssr.get_range());
	EXPECT_EQ(got, expected);
}

TEST(StrSelRange_DragCaret, DragCaret1)
{
	using Dir = dbgutils::Direction;
	using StrSelRange = dbgutils::StringSelectionRange;

	RunDragCaretTest(
		StrSelRange(Dir::LEFT, 2, 5),
		Dir::LEFT, 3, 10,
		StrSelRange(Dir::LEFT, 0, 5)
	);
}

TEST(StrSelRange_DragCaret, DragCaret2)
{
	using Dir = dbgutils::Direction;
	using StrSelRange = dbgutils::StringSelectionRange;

	RunDragCaretTest(
		StrSelRange(Dir::LEFT, 2, 5),
		Dir::RIGHT, 5, 10,
		StrSelRange(Dir::RIGHT, 5, 7)
	);
}

TEST(StrSelRange_DragCaret, DragCaret3)
{
	using Dir = dbgutils::Direction;
	using StrSelRange = dbgutils::StringSelectionRange;

	RunDragCaretTest(
		StrSelRange(Dir::LEFT, 2, 5),
		Dir::RIGHT, 2, 10,
		StrSelRange(Dir::LEFT, 4, 5)
	);
}

TEST(StrSelRange_DragCaret, DragCaret4)
{
	using Dir = dbgutils::Direction;
	using StrSelRange = dbgutils::StringSelectionRange;

	RunDragCaretTest(
		StrSelRange(Dir::LEFT, 2, 5),
		Dir::RIGHT, 3, 10,
		StrSelRange(Dir::LEFT, 5, 5)
	);
}

TEST(StrSelRange_DragCaret, DragCaret5)
{
	using Dir = dbgutils::Direction;
	using StrSelRange = dbgutils::StringSelectionRange;

	RunDragCaretTest(
		StrSelRange(Dir::RIGHT, 5, 9),
		Dir::RIGHT, 1, 10,
		StrSelRange(Dir::RIGHT, 5, 10)
	);
}