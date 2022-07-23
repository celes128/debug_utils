#include "pch.h"
#include <cassert>
#include "Interpreter.h"
#include "string_utils.h"

namespace dbgutils {

	std::wstring Interpreter::execute(const std::wstring &input)
	{
		auto in = input;
		wstr_trim(in);

		std::wstring output;

		for (auto &cmd : m_cmds) {
			std::wstring cmdNames[] = { cmd->Alias(), cmd->Name() };
			for (const auto &name : cmdNames) {
				auto ok = try_cmd(cmd, name, input, &output);
				if (ok) {// Success!
					return output;
				}
			}
		}

		// Failure
		return L"Unknown command";
	}

	bool Interpreter::try_cmd(std::shared_ptr<ICommand> cmd, const std::wstring cmdName, const std::wstring &input, std::wstring *output)
	{
		assert(output != nullptr);

		if (cmdName.length() == 0) {
			return false;
		}

		// Look for the command name in the input string.
		size_t i;
		auto found = wstr_find_substr(input, cmdName, 0, &i);
		if (!found) {
			return false;
		}

		// Compute the command arguments.
		auto rest = input.substr(i + cmdName.length());
		wstr_ltrim(rest);
		auto args = wstr_split(rest);

		// Execute it!
		*output = cmd->execute(args);

		return true;
	}
}