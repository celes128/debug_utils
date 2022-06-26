#include "pch.h"
#include "..\debug_utils\Interpreter.h"
#include "CommandEcho.h"

TEST(Console, interpreter)
{
	dbgutils::CmdList commands{ {std::make_shared<CommandEcho>()} };

	dbgutils::Interpreter interp(commands);

	auto got = interp.execute(L"echo test");
	auto expected = L"test";
	EXPECT_EQ(got, expected);
}