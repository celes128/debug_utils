#include "pch.h"
#include "..\debug_utils\Interpreter.h"
#include "..\debug_utils\string_utils.h"// wstr_concat

// We use a "dummy" echo command to test the interpreter.
// The command simply returns the arguments following the command name.
//
// Example
//
//	> echo hello a b cd     32		// output: hello a b cd 32
//
class CommandEcho : public dbgutils::ICommand {
public:
	CommandEcho()
		: dbgutils::ICommand(L"echo")
	{}

	~CommandEcho() = default;

	std::wstring execute(const dbgutils::CmdArgs &args) override
	{
		return wstr_concat(args, L" ");
	}
};

TEST(Console, interpreter)
{
	dbgutils::CmdList commands{ {std::make_shared<CommandEcho>()} };

	dbgutils::Interpreter interp(commands);

	auto got = interp.execute(L"echo test");
	auto expected = L"test";
	EXPECT_EQ(got, expected);
}