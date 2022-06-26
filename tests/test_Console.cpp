#include "pch.h"
#include "..\debug_utils\Console.h"
#include "CommandEcho.h"

TEST(Console, OneChar)
{
	dbgutils::Console cons;

	cons.handle_character('a');

	auto got = cons.cmdline();
	auto expected = L"a";
	EXPECT_EQ(got, expected);
}

TEST(Console, CharEnter)
{
	dbgutils::Console cons;

	cons.handle_character('a');
	cons.handle_key(VK_RETURN);

	auto got = cons.cmdline();
	auto expected = L"";
	EXPECT_EQ(got, expected);
}

TEST(Console, CharEnterChar)
{
	dbgutils::Console cons;

	cons.handle_character('a');
	cons.handle_key(VK_RETURN);
	cons.handle_character('b');

	auto got = cons.cmdline();
	auto expected = L"b";
	EXPECT_EQ(got, expected);
}

TEST(Console, CharEnterUp)
{
	dbgutils::Console cons;

	cons.handle_character('a');
	cons.handle_key(VK_RETURN);
	cons.handle_key(VK_UP);

	auto got = cons.cmdline();
	auto expected = L"a";
	EXPECT_EQ(got, expected);
}

TEST(Console, CharEnterUpChar)
{
	dbgutils::Console cons;

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
	dbgutils::Console cons;

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
	dbgutils::Console cons;

	cons.handle_character('a');// a
	cons.handle_key(VK_RETURN);// ""
	cons.handle_character('b');// b
	cons.handle_key(VK_UP);// a
	cons.handle_key(VK_DOWN);// b

	auto got = cons.cmdline();
	auto expected = L"b";
	EXPECT_EQ(got, expected);
}

//										UTILITY FUNCTIONS
//

// make_testing_interpreter creates a basic interpreter made for unit testing.
// The interpreter contains an echo command that returns its arguments.
static dbgutils::Interpreter make_testing_interpreter()
{
	dbgutils::CmdList commands{ {std::make_shared<CommandEcho>()} };

	return dbgutils::Interpreter(commands);
}

static void console_write_string_and_execute(dbgutils::Console &cons, const std::wstring &str)
{
	for (const auto &c : str) {
		cons.handle_character(c);
	}

	cons.handle_key(VK_RETURN);// execute the command line
}

static void console_execute_commands(dbgutils::Console &cons, std::vector<std::wstring> cmds)
{
	for (const auto &cmd : cmds) {
		console_write_string_and_execute(cons, cmd);
	}
}

TEST(Console, GetSpecificOutput)
{
	dbgutils::Console cons(make_testing_interpreter());

	console_execute_commands(
		cons,
		std::vector<std::wstring>{
			L"echo oldest",
			L"echo middle",
			L"echo latest"
		}
	);

	EXPECT_EQ(cons.get_output(0), L"latest");
	EXPECT_EQ(cons.get_output(1), L"middle");
	EXPECT_EQ(cons.get_output(2), L"oldest");
}