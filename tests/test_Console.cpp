#include "pch.h"
#include "..\debug_utils\Console.h"

TEST(Console, OneChar)
{
	dbgutils::Console cons(3);

	cons.handle_character('a');

	auto got = cons.cmdline();
	auto expected = L"a";
	EXPECT_EQ(got, expected);
}

TEST(Console, CharEnter)
{
	dbgutils::Console cons(3);

	cons.handle_character('a');
	cons.handle_key(VK_RETURN);

	auto got = cons.cmdline();
	auto expected = L"";
	EXPECT_EQ(got, expected);
}

TEST(Console, CharEnterChar)
{
	dbgutils::Console cons(3);

	cons.handle_character('a');
	cons.handle_key(VK_RETURN);
	cons.handle_character('b');

	auto got = cons.cmdline();
	auto expected = L"b";
	EXPECT_EQ(got, expected);
}

TEST(Console, CharEnterUp)
{
	dbgutils::Console cons(3);

	cons.handle_character('a');
	cons.handle_key(VK_RETURN);
	cons.handle_key(VK_UP);

	auto got = cons.cmdline();
	auto expected = L"a";
	EXPECT_EQ(got, expected);
}

TEST(Console, CharEnterUpChar)
{
	dbgutils::Console cons(3);

	cons.handle_character('a');
	cons.handle_key(VK_RETURN);
	cons.handle_key(VK_UP);
	cons.handle_character('b');

	auto got = cons.cmdline();
	auto expected = L"ab";
	EXPECT_EQ(got, expected);
}

TEST(Console, CharEnterUpDown)
{
	dbgutils::Console cons(3);

	cons.handle_character('a');
	cons.handle_key(VK_RETURN);
	cons.handle_key(VK_UP);
	cons.handle_key(VK_DOWN);

	auto got = cons.cmdline();
	auto expected = L"";
	EXPECT_EQ(got, expected);
}

TEST(Console, CharEnterCharUpDown)
{
	dbgutils::Console cons(3);

	cons.handle_character('a');// a
	cons.handle_key(VK_RETURN);// ""
	cons.handle_character('b');// b
	cons.handle_key(VK_UP);// a
	cons.handle_key(VK_DOWN);// b

	auto got = cons.cmdline();
	auto expected = L"b";
	EXPECT_EQ(got, expected);
}