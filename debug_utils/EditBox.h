#pragma once

#define NOMINMAX
#include <string>
#include <vector>
#include <algorithm>
#include "Direction.h"
#include "Key.h"
#include "Range.h"
#include "StringSelectionRange.h"

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

	// RETURN VALUE
	//	Returns true iff a range was found containing the index.
	//	The range is returned in the output variable iRange.
	bool FindRangeContainingIndex(
		IN size_t index,
		IN const std::vector<StringRange> &ranges,
		OUT size_t *iRange);

	// RETURN VALUE
	//	Returns true iff a range was found.
	//	The range is returned in the output variable iRange.
	bool FindPreviousRangeType(
		IN STRING_RANGE_TYPE type,
		IN size_t start,
		IN const std::vector<StringRange> &ranges,
		OUT size_t *iRange);

	// RETURN VALUE
	//	Returns true iff a range was found.
	//	The range is returned in the output variable iRange.
	bool FindNextRangeType(
		IN STRING_RANGE_TYPE type,
		IN size_t start,
		IN const std::vector<StringRange> &ranges,
		OUT size_t *iRange);

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
		bool handle_key_left(const ModKeyState &mod);
		bool handle_key_right(const ModKeyState &mod);
		bool handle_arrow_key(Direction dir, const ModKeyState &mod);

		bool handle_key_backspace(const ModKeyState &mod);
		bool handle_key_home(const ModKeyState &mod);
		bool handle_key_end(const ModKeyState &mod);

		// set_caret moves the caret at a given position.
		//
		// RETURN VALUE
		//	Returns true iff the caret changed position.
		bool set_caret(size_t position);

		struct Movement {
			// Position (in a string) before the movement.
			size_t	before{ 0 };
			// Position (in a string) after the movement.
			size_t	after{ 0 };

			bool is_null() const { return after == before; }

			Range<size_t> get_range() const
			{
				return Range<size_t>{
					std::min(before, after),// begin
					std::max(before, after)// end
				};
			}

			static const Movement Zero;
		};

		enum CTRL_KEY_STATE {
			CTRL_RELEASED = 0,
			CTRL_PRESSED = 1
		};

		CTRL_KEY_STATE ctrl_key_state(const ModKeyState &mod) const
		{
			return mod.ctrl ? CTRL_PRESSED : CTRL_RELEASED;
		}
		
		// REMARKS
		//	If the ctrl key is pressed, the amount parameter is ignored.
		Movement simulate_caret_movement(Direction dir, size_t amount, CTRL_KEY_STATE ctrl);

		// REMARKS
		//	This function simulates the Ctrl-Left key press behaviour that is found in most text editors.
		//	The behaviour is a bit annoying to explain but the most general case is that the caret moves
		//	to the first character of the word on its left.
		Movement simulate_caret_movement_ctrl_left();
		// Simulate a left movement of at most amount posisitions in the string.
		Movement simulate_caret_movement_left(size_t amount);

		Movement simulate_caret_movement_ctrl_right();
		// Simulate a right movement of at most amount posisitions in the string.
		Movement simulate_caret_movement_right(size_t amount);

		// MovementFromCaretTo returns a Movement object that starts at the caret
		// and goes to the specified destination.
		Movement MovementFromCaretTo(size_t destination);

		void delete_substring(const Range<size_t> &range);

		size_t beginning_of_string() const { return 0; }
		size_t end_of_string() const { return m_str.length(); }

	private:
		// The content of the edit box.
		std::wstring	m_str;
		
		// Position of the caret inside the string.
		size_t			m_caret{ 0 };

		// Current substring selected (when the user holds Shift down).
		StringSelectionRange	m_selection;
	};
}