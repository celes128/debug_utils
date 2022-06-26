#pragma once

#include "..\debug_utils\Interpreter.h"
#include "..\debug_utils\string_utils.h"// wstr_concat

// A "dummy" echo command to test the interpreter and the console output.
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