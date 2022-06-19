#include "pch.h"
#include "CommandLine.h"

namespace dbgutils {

	CommandLine::CommandLine(size_t historyCapacity)
		: m_history(historyCapacity)
	{
		clear_editboxes_and_set_up_new_one();
	}

	void CommandLine::clear_editboxes_and_set_up_new_one()
	{
		// Clear
		m_editboxes.clear();
		m_i = 0;

		// Set up a new one
		m_editboxes.push_back(EditBox());
	}

	const std::wstring & CommandLine::content() const
	{
		return cur_editbox().content();
	}

	size_t CommandLine::caret() const
	{
		return cur_editbox().caret();
	}

	bool CommandLine::handle_character(wchar_t c)
	{
		return cur_editbox().handle_character(c);
	}

	bool CommandLine::handle_key(Key key)
	{
		bool changed = false;

		switch (key) {
		case VK_RETURN:		changed = handle_enter_key(); break;
		case VK_UP:			changed = handle_up_key(); break;
		case VK_DOWN:		changed = handle_down_key(); break;
		default:		break;
		}

		return changed;
	}

	bool CommandLine::handle_enter_key()
	{
		const auto &line = cur_editbox().content();
		if (line.length() == 0) {
			return false;
		}

		// We do not add the editbox string if it the exact same as the latest history entry.
		auto topEntry = reset_history_iter_and_get_top_entry();
		if (topEntry != line) {
			m_history.push(line);
		}

		clear_editboxes_and_set_up_new_one();
		return true;
	}
	
	std::wstring CommandLine::reset_history_iter_and_get_top_entry()
	{
		m_history.reset_iteration();
		m_history.go_to_previous();
		auto entry = m_history.get();
		m_history.reset_iteration();

		return entry;
	}

	bool CommandLine::handle_up_key()
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

		// Here the history iteration ptr points to a (new) previous/older entry.
		m_editboxes.push_back(EditBox(m_history.get()));
		m_i++;
		return true;
	}

	bool CommandLine::handle_down_key()
	{
		if (m_i == 0) {
			return false;
		}

		--m_i;
		return true;
	}

	const EditBox & CommandLine::cur_editbox() const
	{
		assert(m_i < m_editboxes.size());

		return m_editboxes[m_i];
	}

	EditBox & CommandLine::cur_editbox()
	{
		assert(m_i < m_editboxes.size());

		return m_editboxes[m_i];
	}
}