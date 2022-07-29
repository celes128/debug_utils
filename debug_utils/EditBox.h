#pragma once

#include <string>
#include <vector>
#include "Key.h"
#include "Range.h"

namespace dbgutils {

	enum STRING_RANGE_TYPE {
		STRING_RANGE_TYPE_WORD,
		STRING_RANGE_TYPE_SPACE
	};
	struct StringRange {
		STRING_RANGE_TYPE	type{ STRING_RANGE_TYPE_WORD };

		size_t		begin{ 0 };
		size_t		length{ 0 };

		bool contains(size_t x) const
		{
			return begin <= x && x < begin + length;
		}

		static StringRange MakeSpaceRange(size_t beg, size_t len)
		{
			return StringRange{ STRING_RANGE_TYPE_SPACE, beg, len };
		}
		static StringRange MakeWordRange(size_t beg, size_t len)
		{
			return StringRange{ STRING_RANGE_TYPE_WORD, beg, len };
		}
	};

	inline bool operator==(const dbgutils::StringRange &lhs, const dbgutils::StringRange &rhs)
	{
		return lhs.type == rhs.type
			&& lhs.begin == rhs.begin
			&& lhs.length == rhs.length;
	}
	
	inline bool operator!=(const dbgutils::StringRange &lhs, const dbgutils::StringRange &rhs)
	{
		return !(lhs == rhs);
	}



	std::vector<StringRange> ComputeStringRanges(const std::wstring &s);

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
		bool handle_key_ctrl_left();
		bool handle_key_right(const ModKeyState &mod);
		bool handle_key_backspace(const ModKeyState &mod);
		bool handle_key_home(const ModKeyState &mod);
		bool handle_key_end(const ModKeyState &mod);

		void caret_move_left(size_t n);

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