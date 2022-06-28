#include "pch.h"
#include "Console.h"

namespace dbgutils {

	Console::Console(Interpreter interpreter, size_t historyCapacity, size_t outputCapacity)
		: m_interpreter(interpreter)
		, m_history(historyCapacity)
		, m_output(outputCapacity)
	{
		clear_editboxes_and_set_up_new_one();
	}

	void Console::clear_editboxes_and_set_up_new_one()
	{
		// Clear
		m_editboxes.clear();
		m_i = 0;

		// Set up a new one
		m_editboxes.push_back(EditBox());
	}

	const std::wstring & Console::cmdline() const
	{
		return cur_editbox().content();
	}

	size_t Console::caret() const
	{
		return cur_editbox().caret();
	}

	std::wstring Console::get_output(size_t i) const
	{
		assert(0 <= i && i < output_size());

		return m_output.peek(m_output.size() - 1 - i);
	}

	bool Console::handle_character(IN wchar_t c)
	{
		return cur_editbox().handle_character(c);
	}

	bool Console::handle_key(Key key)
	{
		bool changed = false;

		switch (key) {
		case VK_RETURN:	changed = handle_enter_key();
			break;

		case VK_UP:		changed = handle_up_key();
			break;

		case VK_DOWN:	changed = handle_down_key();
			break;
		
		default:		changed = cur_editbox().handle_key(key);
			break;
		}

		return changed;
	}

	bool Console::handle_enter_key()
	{
		m_lastCmdlineStr = cmdline();

		if (cmdline_is_empty()) {
			return false;
		}

		exec_cmdline_and_store_output();

		add_cmdline_to_history_and_reset_iteration();

		clear_editboxes_and_set_up_new_one();

		return true;
	}

	void Console::exec_cmdline_and_store_output()
	{
		auto output = m_interpreter.execute(cmdline());

		m_output.push_back(output);
	}

	void Console::add_cmdline_to_history_and_reset_iteration()
	{
		// We do not add the command line string if it is the exact same
		// as the latest history entry.

		// Get the top entry
		m_history.reset_iteration();
		m_history.go_to_previous();
		auto topEntry = m_history.get();

		// Reset
		m_history.reset_iteration();

		// Add
		if (topEntry != cmdline()) {
			m_history.push(cmdline());
		}
	}

	bool Console::handle_up_key()
	{
		// Go to the previous editbox from our temporary list.
		if (m_i + 1 < m_editboxes.size()) {// Are we at the last editbox of our temporary list?
			++m_i;
			return true;
		}

		// Try to get a previous entry from the history.
		auto ev = m_history.go_to_previous();
		if (ev != ConsoleHistory::ITEREVENT_AT_NEW_ENTRY) {
			// We were at the oldest entry in the history already so we do nothing.
			return false;
		}

		// Here the history iteration ptr points to a previous/older entry
		// that we encounter for the first time.
		m_editboxes.push_back(EditBox(m_history.get()));
		m_i++;
		return true;
	}

	bool Console::handle_down_key()
	{
		if (m_i == 0) {
			return false;
		}

		--m_i;
		return true;
	}

	const EditBox & Console::cur_editbox() const
	{
		assert(m_i < m_editboxes.size());

		return m_editboxes[m_i];
	}

	EditBox & Console::cur_editbox()
	{
		assert(m_i < m_editboxes.size());

		return m_editboxes[m_i];
	}

	bool Console::cmdline_is_empty() const
	{
		return cmdline().length() == 0;
	}
}