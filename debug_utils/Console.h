#pragma once

#include <vector>
#include <string>
#include "Key.h"
#include "ConsoleHistory.h"
#include "EditBox.h"

namespace dbgutils {

	class Console {
	public:
		Console(size_t historyCapacity);

		//		ACCESSORS
		//
		// cmdline returns the string in the command line.
		const std::wstring &cmdline() const;

		// caret returns the position of the caret in the command line string.
		size_t caret() const;

		//		MANIPULATORS
		//
		// All handle_xxx functions return true iff the command line content or the caret changed.
		bool handle_character(wchar_t c);
		bool handle_key(Key key);

	private:
		const EditBox & cur_editbox() const;
		EditBox & cur_editbox();

		bool handle_enter_key();
		std::wstring reset_history_iter_and_get_top_entry();

		bool handle_up_key();
		bool handle_down_key();

		void clear_editboxes_and_set_up_new_one();

	private:
		ConsoleHistory				m_history;

		std::vector<EditBox>		m_editboxes;
		size_t						m_i{ 0 };
	};
}