#include "pch.h"
#include "../debug_utils/EditBox.h"

TEST(EditBox, ContentEmptyStr)
{
	dbgutils::EditBox ed;

	auto got = ed.content();
	auto expected = L"";
	EXPECT_EQ(got, expected);
}

TEST(EditBox, Content)
{
	dbgutils::EditBox ed(L"abc");

	auto got = ed.content();
	auto expected = L"abc";
	EXPECT_EQ(got, expected);
}

TEST(EditBox, InsertChar)
{
	dbgutils::EditBox ed;
	ed.handle_character(L'ㄱ');

	auto got = ed.content();
	auto expected = L"ㄱ";
	EXPECT_EQ(got, expected);
}

TEST(EditBox, CharLeftChar)
{
	dbgutils::EditBox ed;
	ed.handle_character(L'b');
	ed.handle_key(VK_LEFT);
	ed.handle_character(L'a');

	auto got = ed.content();
	auto expected = L"ab";
	EXPECT_EQ(got, expected);
}

TEST(EditBox, CharLeftCharRightChar)
{
	dbgutils::EditBox ed;
	ed.handle_character(L'b');
	ed.handle_key(VK_LEFT);
	ed.handle_character(L'a');
	ed.handle_key(VK_RIGHT);
	ed.handle_character(L'c');

	auto got = ed.content();
	auto expected = L"abc";
	EXPECT_EQ(got, expected);
}

TEST(EditBox, Backspace)
{
	dbgutils::EditBox ed;
	ed.handle_character(L'b');
	ed.handle_key(VK_BACK);
	ed.handle_character(L'a');

	auto got = ed.content();
	auto expected = L"a";
	EXPECT_EQ(got, expected);
}

TEST(EditBox, CharBackspaceCharLeftChar)
{
	dbgutils::EditBox ed;
	ed.handle_character(L'c');
	ed.handle_key(VK_BACK);
	ed.handle_character(L'b');
	ed.handle_key(VK_LEFT);
	ed.handle_character(L'a');

	auto got = ed.content();
	auto expected = L"ab";
	EXPECT_EQ(got, expected);
}