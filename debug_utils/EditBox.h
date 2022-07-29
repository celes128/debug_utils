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

		bool handle_key(Key key, const ModKeyState &mod = ModKeyState());

	private:
		// selection_range returns the selection range defined by the caret and the mark.
		// The range can be empty.
		// PRECONDITIONS
		//	m_selecting == true
		//Range<size_t> selection_range() const;

		bool handle_key_left(const ModKeyState &mod);
		bool handle_key_right(const ModKeyState &mod);
		bool handle_key_backspace(const ModKeyState &mod);
		bool handle_key_home(const ModKeyState &mod);
		bool handle_key_end(const ModKeyState &mod);

		bool caret_at_begin() const { return m_caret <= 0; }
		void caret_move_left(size_t n);

		// DEFINITION: space character
		//	A 'space character' is any wchar_t c such that std::isspace(c) == true. 
		// DEFINITION: word
		//	A 'word' is a contiguous sequence of (at least one) non-space characters.
		bool caret_inside_word() const;

		// caret_move_from_inside_to_word_begin moves the caret,
		// located inside a word or on its last character,
		// to the left until it reaches the beginning of the word.
		// The function does not move the caret if the caret is not inside a word.
		void caret_move_from_inside_to_word_begin();

		// caret_move_to_previous_word_begin tries to find the previous word and
		// if it exists, moves the caret to the beginning of it.
		void caret_move_to_previous_word_begin();

		// caret_move_from_inside_to_space_begin moves the caret,
		// located inside a space sequence or on its right-most space character,
		// to the lest until it reaches the beginning of this space sequence.
		void caret_move_from_inside_to_space_begin();

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