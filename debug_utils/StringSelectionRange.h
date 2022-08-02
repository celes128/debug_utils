#pragma once

#include "Range.h"
#include "Direction.h"

namespace dbgutils {

	class StringSelectionRange {
	public:
		friend bool operator==(const StringSelectionRange &lhs, const StringSelectionRange &rhs);
		friend bool operator!=(const StringSelectionRange &lhs, const StringSelectionRange &rhs);

		StringSelectionRange(Direction caretSide = Direction::LEFT, const Range<size_t> &range = Range<size_t>(0, 0))
			: m_range(range)
			, m_caretSide(caretSide)
		{}

		StringSelectionRange(Direction caretSide, size_t begin, size_t end)
			: m_range(begin, end)
			, m_caretSide(caretSide)
		{}

		//					ACCESSORS
		//
		
		auto get_range() const { return m_range; }
		auto get_caret_side() const { return m_caretSide; }
		bool empty() const { return m_range.empty(); }

		// Get the end points of the range.
		size_t get_caret() const;
		size_t get_other_end() const;

		// Get the left or right end points of the range.
		size_t get_left() const { return m_range.begin(); }
		size_t get_right() const { return m_range.end(); }
		size_t get_end_point(Direction dir) const;

		//					MANIPULATORS
		//

		// drag_caret updates the selection range by moving the caret.
		// The caret is dragged by at most <amount> positions in the specified direction.
		// The caret cannot go below 0 nor above <end>.
		//
		// CODE SAMPLE
		//	auto str = "Hello, world!";
		//	StringSelectionRange selection;
		//	selection.drag_caret(Diretion::RIGHT, 7, str.length());
		//
		void drag_caret(Direction dir, size_t amount, size_t end);

		//	collapse reduce the selection range to zero and put the caret either on the left
		//	or right end point.
		//
		// RETURN VALUE
		//	Returns true iff the range was not empty before the call.
		bool collapse(Direction dir);

	private:
		Range<size_t>	m_range;
		Direction		m_caretSide;
	};

	inline bool operator==(const StringSelectionRange &lhs, const StringSelectionRange &rhs)
	{
		return lhs.m_range.begin() == rhs.m_range.begin()
			&& lhs.m_range.end() == rhs.m_range.end()
			&& lhs.m_caretSide == rhs.m_caretSide;
	}
	
	inline bool operator!=(const StringSelectionRange &lhs, const StringSelectionRange &rhs)
	{
		return !(lhs == rhs);
	}
}