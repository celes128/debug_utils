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

	bool FindPreviousRangeType(
		IN STRING_RANGE_TYPE type,
		IN size_t start,
		IN const std::vector<StringRange> &ranges,
		OUT size_t *iRange)
	{
		assert(iRange != nullptr);
		const auto n = ranges.size();
		assert(0 <= start && start <= n);// large inequality here

		for (size_t i = start; i > 0; i--) {
			if (ranges[i-1].type == type) {
				if (iRange) {
					*iRange = i-1;
				}
				return true;
			}
		}

		return false;
	}

	bool FindNextRangeType(
		IN STRING_RANGE_TYPE type,
		IN size_t start,
		IN const std::vector<StringRange> &ranges,
		OUT size_t *iRange)
	{
		assert(iRange != nullptr);
		const auto n = ranges.size();
		assert(0 <= start && start <= n);// large inequality here

		for (size_t i = start; i + 1 < ranges.size(); i++) {
			if (ranges[i + 1].type == type) {
				if (iRange) {
					*iRange = i + 1;
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
		auto caretInARange = FindRangeContainingIndex(IN m_caret, IN ranges, OUT &i);
		if (!caretInARange) {
			// Case where the caret is at the end of the string, hence does not belong
			// to any range. We put a sentinel value to i that will be handled by
			// the following code below.
			i = ranges.size();
		}

		// If the caret is in a word range but not at the begining,
		// we move it to the beginning of this range.
		if (caretInARange) {
			const auto &caretRange = ranges[i];
			if (caretRange.type == STRING_RANGE_TYPE_WORD && m_caret != caretRange.begin) {
				m_caret = caretRange.begin;
				return prev != m_caret;
			}
		}

		// From now on, we try to find a range to move the caret to.
		// j is the index of this range.
		size_t j;

		// Then we look for a word range on the left.
		auto prevWordRangeFound = FindPreviousRangeType(STRING_RANGE_TYPE_WORD, i, ranges, &j);
		if (prevWordRangeFound) {
			m_caret = ranges[j].begin;
			return prev != m_caret;
		}

		// Then we look for a space range on the left.
		auto prevSpaceRangeFound = FindPreviousRangeType(STRING_RANGE_TYPE_SPACE, i, ranges, &j);
		if (prevSpaceRangeFound) {
			m_caret = ranges[j].begin;
			return prev != m_caret;
		}

		// If no previous range could be found then it means that
		// the string is empty so we put the caret at the beginning of it.
		m_caret = 0;
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
		if (mod.ctrl) {
			return handle_key_ctrl_right();
		}

		if (m_caret >= m_str.length()) {
			return false;
		}

		++m_caret;
		return true;
	}

	bool EditBox::handle_key_ctrl_right()
	{
		// Save the caret in order to check, before returning, if it moved.
		auto prev = m_caret;

		// Determine the word and space ranges in the input string.
		auto ranges = ComputeStringRanges(m_str);

		// Look for the range that contains the caret.
		size_t i = 0;
		auto caretInARange = FindRangeContainingIndex(IN m_caret, IN ranges, OUT &i);
		if (!caretInARange) {
			// Case where the caret is at the end of the string, hence does not belong
			// to any range. We put a sentinel value to i that will be handled by
			// the following code below.
			i = ranges.size();
		}

		// Try to go to the next word's beginning.
		size_t j;
		auto nextWordRangeFound = FindNextRangeType(STRING_RANGE_TYPE_WORD, i, ranges, &j);
		if (nextWordRangeFound) {
			m_caret = ranges[j].begin;
			return prev != m_caret;
		}

		// Else go to the end.
		m_caret = m_str.length();
		return prev != m_caret;
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