#include "pch.h"
#include "EditBox.h"
#include <cctype> // std::isspace
#include <cassert>

namespace dbgutils {

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
		size_t	begin;

		// IMPORTANT NOTE
		// The inequality is indeed large (<=); it is not an error.
		// The value m_sLen for the loop index i is a sentinel and
		// is handled by GenerateEvent.
		for (size_t i = 0; i <= m_sLen; i++) {
			auto ev = GenerateEvent(i);
			switch (ev) {
			case Event::Initialize: {
				begin = 0;
			}break;

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
		auto prev = m_caret;

		if (!mod.ctrl) {
			caret_move_left(1);
			return prev != m_caret;
		}

		auto inside = caret_inside_word();
		if (inside) {
			caret_move_from_inside_to_word_begin();
			return prev != m_caret;
		}

		caret_move_to_previous_word_begin();
		return prev != m_caret;
	}

	void EditBox::caret_move_left(size_t n)
	{
		for(; !caret_at_begin() && n > 0; n--) {
			--m_caret;
		}
	}

	bool EditBox::caret_inside_word() const
	{
		if (caret_at_begin()) {
			return false;
		}

		auto caretOnSpace = std::isspace(m_str[m_caret]);
		if (caretOnSpace) {
			return false;
		}

		// Here we know that:
		//	- there is at least one character on the left of the caret (a space or not a space)
		//	- the caret is not on a space character.
		// We can safely access m_str[m_caret - 1].
		
		auto spaceOnLeftOfCaret = std::isspace(m_str[m_caret - 1]);
		if (spaceOnLeftOfCaret) {
			return false;
		}

		// Here both the caret and caret-1 point to a non-space character
		// meaning that the caret is inside a word.
		return true;
	}

	void EditBox::caret_move_from_inside_to_word_begin()
	{
		while (caret_inside_word()) {
			caret_move_left(1);
		}
	}

	void EditBox::caret_move_to_previous_word_begin()
	{
		// Save the caret to restore it later if the operation fails,
		// because there is no word on the left.
		auto savedCaret = m_caret;

		// If the caret is inside a word, try to move it outside.
		if (caret_inside_word()) {
			caret_move_from_inside_to_word_begin();

			// Move the caret to the last space character of
			// the contiguous space sequence on the left.
			if (m_caret > 0) {
				m_caret--;
			}

		}

		// Failure: The caret was on the first word of the string and
		// there is no space on the left.
		if (caret_at_begin()) {
			m_caret = savedCaret;
			return;
		}
		
		caret_move_from_inside_to_space_begin();

		// Failure: The caret was on the first word of the string.
		// (And the string begins with at least one space.)
		if (caret_at_begin()) {
			m_caret = savedCaret;
			return;
		}

		// At this point we know that there is a previous word.
		// Move the caret to the last character of the word on the left.
		m_caret--;

		caret_move_from_inside_to_word_begin();
	}

	void EditBox::caret_move_from_inside_to_space_begin()
	{
		for (; m_caret >= 1; m_caret--) {
			auto leftCharIsASpace = std::isspace(m_str[m_caret - 1]);
			if (!leftCharIsASpace) {
				return;
			}
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