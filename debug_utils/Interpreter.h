#pragma once
#include <vector>
#include <memory>
#include <string>

namespace dbgutils {

	using CmdArgs = std::vector<std::wstring>;

	class ICommand {
	public:
		ICommand(const std::wstring &name, const std::wstring &alias = L"")
			: m_name(name)
			, m_alias(alias)
		{}
		
		virtual ~ICommand() = default;

		//		MANIPULATORS
		//
		const auto &Name() const { return m_name; }
		const auto &Alias() const { return m_alias; }

		//		MANIPULATORS
		//
		virtual std::wstring execute(const CmdArgs &args) = 0;

	private:
		// The usual name e.g. print for a print command.
		std::wstring	m_name;

		// Optional name, usally shorter e.g. p for print.
		// An empty string if no alias.
		std::wstring	m_alias;
	};


	using CmdList = std::vector<std::shared_ptr<dbgutils::ICommand>>;

	class Interpreter {
	public:
		Interpreter(const CmdList &cmds)
			: m_cmds(cmds)
		{}

		//		MANIPULATORS
		//
		std::wstring execute(const std::wstring &input);

	private:
		// try_cmd attempts to execute the command if its name matches the first token in the input.
		// RETURN VALUE
		//	Returns true iff the command name matched the first token of the input.
		bool try_cmd(const std::wstring cmdName, const std::wstring &input, std::wstring *output);

	private:
		CmdList		m_cmds;
	};
}