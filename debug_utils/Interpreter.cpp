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

		for (const auto &cmd : m_cmds) {
			std::wstring cmdNames[] = { cmd->Alias(), cmd->Name() };
			for (const auto &name : cmdNames) {
				auto ok = try_cmd(name, input, &output);
				if (ok) {// Success!
					return output;
				}
			}
		}

		// Failure
		return L"Unknown command";
	}

	bool Interpreter::try_cmd(const std::wstring cmdName, const std::wstring &input, std::wstring *output)
	{
		assert(output != nullptr);

		if (cmdName.length() == 0) {
			return false;
		}

		size_t i;
		auto found = wstr_find_substr(input, cmdName, 0, &i);
		if (!found) {
			return false;
		}

		// Return the end of the input starting right after the command name.
		*output = input.substr(i + cmdName.length());
		wstr_ltrim(*output);
		return true;
	}
}