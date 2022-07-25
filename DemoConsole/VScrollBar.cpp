#include "VScrollBar.h"
#include <algorithm>

namespace gui {

	VScrollBar::VScrollBar(
		const Point2dF &pos, const SizeF &size,
		const RectF &padding,
		const D2D1_COLOR_F &cursorColor, const D2D1_COLOR_F &bgColor
	)
		: m_pos(pos)
		, m_size(size)
		, m_padding(padding)
		, m_cursorColor(cursorColor)
		, m_bgColor(bgColor)
		, m_cursorLocalY(0.f)
	{
		UpdateCursorSize();
	}

	void VScrollBar::UpdateCursorSize()
	{
		auto w = std::max(m_size.width - (m_padding.left + m_padding.right), 0.f);
		auto h = std::max(m_size.height - (m_padding.top + m_padding.bottom), 0.f);

		m_cursorSize = { w,h };
	}

	void VScrollBar::SetCursorHeightPercent(float p)
	{
		assert(0 <= p && p <= 1.f);

		auto maxHeight = m_size.height - (m_padding.top + m_padding.bottom);
		auto h = p * maxHeight;

		m_cursorSize.height = h;
	}

	void VScrollBar::SetCursorPositionPercent(float p)
	{
		assert(0 <= p && p <= 1.f);
	
		auto bottom = m_size.height - (m_padding.top + m_padding.bottom);
		auto maxY = bottom - m_cursorSize.height;

		m_cursorLocalY = p * maxY;
	}

	void VScrollBar::Render(Renderer &ren)
	{
		// void DrawBackgroundBox(Renderer &ren);
		{
			auto rect = RectF_FromPointAndSize(m_pos, m_size);
			ren.solidBrush->SetColor(m_bgColor);
			ren.renderTarget->FillRectangle(rect, ren.solidBrush);
		}

		// void DrawCursor(Renderer &ren);
		{
			auto offset = m_pos + Point2dF{ m_padding.left, m_padding.top };

			auto cursorLocalPos = Point2dF{ 0.f, m_cursorLocalY };

			auto p = offset + cursorLocalPos;
			
			auto rect = RectF_FromPointAndSize(p, m_cursorSize);

			ren.solidBrush->SetColor(m_cursorColor);
			ren.renderTarget->FillRectangle(rect, ren.solidBrush);
		}
	}
}