#pragma once

#include <string>
#include "Key.h"
#include "Range.h"

namespace dbgutils {

	class EditBox {
	public:
		EditBox(const std::wstring str = L"");

		//			ACCESSORS
		//

		// content returns the string contained in the edit box.
		const auto &content() const { return m_str; }

		// caret returns the position of the caret in the content string.
		const auto caret() const { return m_caret; }

		//			MANIPULATORS
		//

		// NOTE
		//	All the handle_xxx functions return true iff the content string or the caret changed.

		// handle_character receives a printable unicode character to insert at the caret.
		bool handle_character(wchar_t c);

		bool handle_key(Key key);

	private:
		// selection_range returns the selection range defined by the caret and the mark.
		// The range can be empty.
		// PRECONDITIONS
		//	m_selecting == true
		//Range<size_t> selection_range() const;

		bool handle_key_left();
		bool handle_key_right();
		bool handle_key_backspace();
		bool handle_key_home();
		bool handle_key_end();

	private:
		// The content of the edit box.
		std::wstring	m_str;
		
		// Position of the caret inside the string.
		size_t			m_caret{ 0 };

		//// NOT IMPLEMENTED
		//// Selection state
		//// When the user holds shift and moves the caret, it creates a selection region
		//// delimited by the caret and the mark.
		//bool			m_selecting{ false };
		//size_t		m_mark{ 0 };
	};
}