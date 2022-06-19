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
		const auto &content() const { return m_str; }
		const auto caret() const { return m_caret; }

		//			MANIPULATORS
		//

		// All the handle_xxx functions return true iff the content or the caret changed.
		bool handle_character(wchar_t c);
		bool handle_key(Key key);

	private:
		//Range<size_t> selection_range() const;

		bool handle_key_left();
		bool handle_key_right();
		bool handle_key_backspace();
		bool handle_key_return();
		bool handle_key_home();
		bool handle_key_end();

	private:
		// The content of the edit box.
		std::wstring	m_str;
		
		// Saved content after the enter is pressed.
		std::wstring	m_result;

		// Position of the caret inside the string.
		size_t			m_caret{ 0 };

		//// NOT IMPLEMENTED
		//// Selection state - when the user holds shift and moves the caret.
		//bool			m_selecting{ false };
		//size_t		m_mark{ 0 };
	};
}