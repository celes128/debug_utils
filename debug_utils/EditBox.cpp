#include "pch.h"
#include "EditBox.h"

namespace dbgutils {

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

	bool EditBox::handle_key(Key key)
	{
		auto changed = false;

		switch (key) {
		case VK_LEFT:		changed = handle_key_left(); break;
		case VK_RIGHT:		changed = handle_key_right(); break;
		case VK_BACK:		changed = handle_key_backspace(); break;
		case VK_HOME:		changed = handle_key_home(); break;
		case VK_END:		changed = handle_key_end(); break;

		default: break;
		}

		return changed;
	}

	bool EditBox::handle_key_left()
	{
		if (m_caret <= 0) {
			return false;
		}

		--m_caret;
		return true;
	}

	bool EditBox::handle_key_right()
	{
		if (m_caret >= m_str.length()) {
			return false;
		}

		++m_caret;
		return true;
	}

	bool EditBox::handle_key_backspace()
	{
		if (m_caret <= 0) {
			return false;
		}

		--m_caret;
		m_str.erase(m_caret, 1);
		return true;
	}

	bool EditBox::handle_key_home()
	{
		if (m_str.length() == 0 || m_caret == 0) {
			return false;
		}

		m_caret = 0;
		return true;
	}

	bool EditBox::handle_key_end()
	{
		if (m_caret >= m_str.length()) {
			return false;
		}

		m_caret = m_str.length();
		return true;
	}
}