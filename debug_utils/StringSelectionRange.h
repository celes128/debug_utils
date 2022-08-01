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

		//					MANIPULATORS
		//

		void drag_caret(Direction dir, size_t amount, size_t end);

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