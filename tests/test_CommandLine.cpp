#include "pch.h"
#include "../debug_utils/CommandLine.h"

TEST(CommandLine, OneCharacter)
{
	dbgutils::CommandLine cl(3);

	cl.handle_character('a');

	auto got = cl.content();
	auto expected = L"a";
	EXPECT_EQ(got, expected);
}

TEST(CommandLine, CharEnter)
{
	dbgutils::CommandLine cl(3);

	cl.handle_character('a');
	cl.handle_key(VK_RETURN);

	auto got = cl.content();
	auto expected = L"";
	EXPECT_EQ(got, expected);
}

TEST(CommandLine, CharEnterChar)
{
	dbgutils::CommandLine cl(3);

	cl.handle_character('a');
	cl.handle_key(VK_RETURN);
	cl.handle_character('b');

	auto got = cl.content();
	auto expected = L"b";
	EXPECT_EQ(got, expected);
}

TEST(CommandLine, CharEnterUp)
{
	dbgutils::CommandLine cl(3);

	cl.handle_character('a');
	cl.handle_key(VK_RETURN);
	cl.handle_key(VK_UP);

	auto got = cl.content();
	auto expected = L"a";
	EXPECT_EQ(got, expected);
}

TEST(CommandLine, CharEnterUpChar)
{
	dbgutils::CommandLine cl(3);

	cl.handle_character('a');
	cl.handle_key(VK_RETURN);
	cl.handle_key(VK_UP);
	cl.handle_character('b');

	auto got = cl.content();
	auto expected = L"ab";
	EXPECT_EQ(got, expected);
}

TEST(CommandLine, CharEnterUpDown)
{
	dbgutils::CommandLine cl(3);

	cl.handle_character('a');
	cl.handle_key(VK_RETURN);
	cl.handle_key(VK_UP);
	cl.handle_key(VK_DOWN);

	auto got = cl.content();
	auto expected = L"";
	EXPECT_EQ(got, expected);
}

TEST(CommandLine, CharEnterCharUpDown)
{
	dbgutils::CommandLine cl(3);

	cl.handle_character('a');// a
	cl.handle_key(VK_RETURN);// ""
	cl.handle_character('b');// b
	cl.handle_key(VK_UP);// a
	cl.handle_key(VK_DOWN);// b

	auto got = cl.content();
	auto expected = L"b";
	EXPECT_EQ(got, expected);
}