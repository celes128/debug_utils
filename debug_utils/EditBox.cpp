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
		auto prev = m_caret;

		auto ranges = ComputeStringRanges(m_str);

		// Look for the range that contains the caret.
		auto found = false;
		size_t i = 0;
		for (; i < ranges.size(); i++) {
			if (ranges[i].contains(m_caret)) {
				found = true;
				break;
			}
		}

		// Not found <=> The string is empty.
		if (!found) {		
			return false;
		}

		const auto &range = ranges[i];
		if (range.type == STRING_RANGE_TYPE_WORD && m_caret != range.begin) {
			m_caret = range.begin;
		}
		else {
			// Move the caret to the beginning of the previous word if it exists.
			switch (range.type) {
			case STRING_RANGE_TYPE_SPACE: {
				if (i >= 1) {
					m_caret = ranges[i - 1].begin;
				}
			}break;
			case STRING_RANGE_TYPE_WORD: {
				if (i >= 2) {
					m_caret = ranges[i - 2].begin;
				}
			}break;
			}
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