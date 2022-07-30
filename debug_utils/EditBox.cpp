#include "pch.h"
#include "EditBox.h"
#include <cctype> // std::isspace
#include <cassert>

namespace dbgutils {
	
	//	class:				StringRangeGenerator
	//
	//	The StringRangeGenerator is a helper class used to implement
	//	the public function	ComputeStringRanges.
	
	class StringRangeGenerator {
	public:
		StringRangeGenerator(const std::wstring *s)
			: m_s(s)
		{
			assert(s != nullptr);
			m_sLen = s->length();
		}

		std::vector<StringRange> GenerateRanges();

	private:
		enum Event {
			Unknown,
			Initialize,
			WordRangeLeft,
			WordRangeContinued,
			SpaceRangeLeft,
			SpaceRangeContinued
		};

		// GenerateEvent returns an event based on the index i in the input string.
		Event GenerateEvent(size_t i) const;

	private:
		const std::wstring	*m_s;
		size_t				m_sLen;
	};

	std::vector<StringRange> StringRangeGenerator::GenerateRanges()
	{
		if (m_s->empty()) {
			return {};
		}

		std::vector<StringRange> ranges;

		// Beginning of the current range.
		size_t	begin = 0;

		// IMPORTANT NOTE
		// The inequality is indeed large (<=); it is not an error.
		// The value m_sLen for the loop index i is a sentinel and
		// is handled by GenerateEvent.
		for (size_t i = 0; i <= m_sLen; i++) {
			auto ev = GenerateEvent(i);
			switch (ev) {
			case Event::WordRangeLeft: {
				ranges.push_back(StringRange::MakeWordRange(begin, i - begin));
				begin = i;
			}break;
			
			case Event::SpaceRangeLeft: {
				ranges.push_back(StringRange::MakeSpaceRange(begin, i - begin));
				begin = i;
			}break;

			case Event::Unknown: {
				assert(false && "Should never happen.");
			}break;

			default:// Nothing to do.
				break;
			}
		}

		return ranges;
	}

	StringRangeGenerator::Event StringRangeGenerator::GenerateEvent(size_t i) const
	{
		assert(m_sLen != 0);
		assert(i <= m_sLen && "Index out of range of the input string.");

		// Did we enter the string?
		if (i == 0) {
			return Event::Initialize;
		}

		auto prev = (*m_s)[i-1];

		// Did we quit the string?
		if (i == m_sLen) {
			if (std::isspace(prev)) {
				return Event::SpaceRangeLeft;
			}
			else {
				return Event::WordRangeLeft;
			}
		}

		// Here we can access [i] since 0 <= i < m_sLen.
		auto cur = (*m_s)[i];

		if (std::isspace(prev)) {
			if (std::isspace(cur)) {
				return Event::SpaceRangeContinued;
			}
			else {
				return Event::SpaceRangeLeft;
			}
		}
		
		// Here, prev is not a space.
		if (std::isspace(cur)) {
			return Event::WordRangeLeft;
		}
		else {
			return Event::WordRangeContinued;
		}
	}

	std::vector<StringRange> ComputeStringRanges(const std::wstring &s)
	{
		StringRangeGenerator	generator(&s);
		return generator.GenerateRanges();
	}

	bool FindRangeContainingIndex(
		IN size_t index,
		IN const std::vector<StringRange> &ranges,
		OUT size_t *iRange)
	{
		assert(iRange != nullptr);

		for (size_t i = 0; i < ranges.size(); i++) {
			if (ranges[i].contains(index)) {
				if (iRange) {
					*iRange = i;
				}
				return true;
			}
		}

		return false;
	}


	//	class:				EditBox
	//
	//

	EditBox::EditBox(const std::wstring str)
		: m_str(str)
		, m_caret(str.length())
	{}

	bool EditBox::handle_character(wchar_t c)
	{
		m_str.insert(m_caret, 1, c);
		++m_caret;

		return true;
	}

	bool EditBox::handle_key(Key key, const ModKeyState &mod)
	{
		auto changed = false;

		switch (key) {
		case VK_LEFT:		changed = handle_key_left(mod); break;
		case VK_RIGHT:		changed = handle_key_right(mod); break;
		case VK_BACK:		changed = handle_key_backspace(mod); break;
		case VK_HOME:		changed = handle_key_home(mod); break;
		case VK_END:		changed = handle_key_end(mod); break;

		default: break;
		}

		return changed;
	}

	bool EditBox::handle_key_left(const ModKeyState &mod)
	{
		if (mod.ctrl) {
			return handle_key_ctrl_left();
		}

		auto prev = m_caret;
		caret_move_left(1);
		return prev != m_caret;
	}

	bool EditBox::handle_key_ctrl_left()
	{
		// Save the caret in order to check, before returning, if it moved.
		auto prev = m_caret;

		// Determine the word and space ranges in the input string.
		auto ranges = ComputeStringRanges(m_str);

		// Look for the range that contains the caret.
		size_t i = 0;
		auto found = FindRangeContainingIndex(IN m_caret, IN ranges, OUT &i);

		// Special case: the caret is not in a range <=> the caret is at the end of the string.
		if (!found) {
			auto n = ranges.size();
			switch (n) {
			case 0:
				// The string is empty. Do not move the caret.
				return false;
				break;

			case 1:
				// The string has only one range.
				// Put the caret at the beginning of the string.
				m_caret = 0;
				return prev != m_caret;

			default:
				// There are at least two ranges in the string. There are only two cases:
				//	(1) the string ends in { ..., word range, space range } or,
				//	(2) the string ends in { ..., space range, word range }.
				const auto &secondToLast = ranges[n - 2];
				const auto &last = ranges[n - 1];
				if (last.type == STRING_RANGE_TYPE_SPACE) { // (1)
					m_caret = secondToLast.begin;
				}
				else if (last.type == STRING_RANGE_TYPE_WORD) { // (2)
					m_caret = last.begin;
				}
				return prev != m_caret;
			}
		}

		// Special case 2: the caret is in a range and #ranges == 1.
		if (ranges.size() == 1) {
			m_caret = 0;
			return prev != m_caret;
		}

		// The most general case: the caret is in a range and #ranges >= 2.
		const auto &range = ranges[i];

		// When the caret is inside a word but not at its first character,
		// move the caret to the first character.
		if (range.type == STRING_RANGE_TYPE_WORD && m_caret != range.begin) {
			m_caret = range.begin;
			return prev != m_caret;
		}
		
		// Else try to move the caret to the previous word.
		// It may not exist hence the "try".
		return caret_move_to_prev_word_begin(ranges, i);
	}

	bool EditBox::caret_move_to_prev_word_begin(const std::vector<StringRange> &ranges, size_t iCaretRange)
	{
		const auto n = ranges.size();
		assert(iCaretRange <= n);

		auto prev = m_caret;

		const auto &range = ranges[iCaretRange];
		switch (range.type) {
		case STRING_RANGE_TYPE_SPACE: {
			if (iCaretRange >= 1) {
				m_caret = ranges[iCaretRange - 1].begin;
			}
		}break;
		case STRING_RANGE_TYPE_WORD: {
			if (iCaretRange >= 2) {
				m_caret = ranges[iCaretRange - 2].begin;
			}
		}break;
		}

		return prev != m_caret;
	}

	void EditBox::caret_move_left(size_t n)
	{
		for(; m_caret >= 1 && n > 0; n--) {
			--m_caret;
		}
	}

	bool EditBox::handle_key_right(const ModKeyState &mod)
	{
		if (m_caret >= m_str.length()) {
			return false;
		}

		++m_caret;
		return true;
	}

	bool EditBox::handle_key_backspace(const ModKeyState &mod)
	{
		if (m_caret <= 0) {
			return false;
		}

		--m_caret;
		m_str.erase(m_caret, 1);
		return true;
	}

	bool EditBox::handle_key_home(const ModKeyState &mod)
	{
		if (m_str.length() == 0 || m_caret == 0) {
			return false;
		}

		m_caret = 0;
		return true;
	}

	bool EditBox::handle_key_end(const ModKeyState &mod)
	{
		if (m_caret >= m_str.length()) {
			return false;
		}

		m_caret = m_str.length();
		return true;
	}
}