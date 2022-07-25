#include "Scroller.h"
#include <cassert>
#include <algorithm>

namespace gui {

	Scroller::Scroller(float spaceLength, float viewLength, float viewPosPercent)
		: m_spaceLength(spaceLength)
		, m_viewLength(viewLength)
		, m_viewPosition(viewPosPercent * m_spaceLength)
	{
		assert(spaceLength >= 0.f);
		assert(viewLength >= 0.f);
		assert(0.f <= viewPosPercent && viewPosPercent <= 1.f);
	}

	float Scroller::GetViewPositionPercentage() const
	{
		auto maxPos = m_spaceLength - m_viewLength;

		return m_viewPosition / maxPos;
	}

	float Scroller::ScrollUp(float displacement, bool *moved)
	{
		const auto previousPosition = m_viewPosition;

		if (displacement > 0.f) {
			// Move the view backward <=> up <=> left.
			auto maxMvt = std::max(m_viewPosition, 0.f);
			auto amount = std::min(displacement, maxMvt);
			m_viewPosition -= amount;
		}

		if (moved) {
			*moved = m_viewPosition != previousPosition;
		}

		return m_viewPosition;
	}

	float Scroller::ScrollDown(float displacement, bool *moved)
	{
		const auto previousPosition = m_viewPosition;

		if (displacement > 0.f) {
			// Move the view forward <=> down <=> right.
			auto maxMvt = std::max(m_spaceLength - (m_viewPosition + m_viewLength), 0.f);
			auto amount = std::min((float)displacement, maxMvt);
			m_viewPosition += amount;
		}

		if (moved) {
			*moved = m_viewPosition != previousPosition;
		}

		return m_viewPosition;
	}

	void Scroller::SetSpaceLength(float length)
	{
		if (length <= 0.f) {
			return;
		}

		if (length < m_spaceLength) {
			m_viewPosition = std::min(length, m_viewPosition);
		}

		m_spaceLength = length;
	}
}