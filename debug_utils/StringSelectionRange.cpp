#include "pch.h"
#include "StringSelectionRange.h"
#include <cassert>
#include <algorithm>

namespace dbgutils {

	static size_t MovePoint(size_t point, Direction dir, size_t amount, size_t end)
	{
		assert(0 <= point && point <= end);
		assert(dir == LEFT || dir == RIGHT);

		if (dir == LEFT) {
			auto maxLeftDisplacement = point - 0;
			auto displacement = std::min(amount, maxLeftDisplacement);
			return point - displacement;
		}
		else if (dir == RIGHT) {
			auto maxRightDisplacement = end - point;
			auto displacement = std::min(amount, maxRightDisplacement);
			return point + displacement;
		}

		assert(false && "Direction not handled.");
		return point;
	}

	void StringSelectionRange::drag_caret(Direction dir, size_t amount, size_t end)
	{
		// Compute the two end points of the new range.
		auto newCaret = MovePoint(get_caret(), dir, amount, end);
		auto otherEndPoint = get_other_end();

		// Update the member variables based on these new end points.
		if (m_caretSide == LEFT) {
			if (newCaret > otherEndPoint) {
				*this = StringSelectionRange(RIGHT, otherEndPoint, newCaret);
			}
			else {
				m_range = Range<size_t>(newCaret, otherEndPoint);
			}
		}
		else if (m_caretSide == RIGHT) {
			if (newCaret < otherEndPoint) {
				*this = StringSelectionRange(LEFT, newCaret, otherEndPoint);
			}
			else {
				m_range = Range<size_t>(otherEndPoint, newCaret);
			}
		}
	}

	size_t StringSelectionRange::get_caret() const
	{
		if (m_caretSide == LEFT) {
			return m_range.begin();
		}
		else {
			return m_range.end();
		}
	}

	size_t StringSelectionRange::get_other_end() const
	{
		if (m_caretSide == LEFT) {
			return m_range.end();
		}
		else {
			return m_range.begin();
		}
	}
}