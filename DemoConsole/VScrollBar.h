#pragma once

#include <cassert>
#include "Renderer.h"
#include "geom.h"

namespace gui {

	class VScrollBar {
	public:
		VScrollBar(
			const Point2dF &pos,
			const SizeF &size,
			const RectF &padding,
			const D2D1_COLOR_F &cursorColor,
			const D2D1_COLOR_F &bgColor);


		//				ACCESSORS
		//

		auto GetPosition() const { return m_pos; }

		auto GetBoundingBox() const
		{
			return RectF_FromPointAndSize(m_pos, m_size);
		}
		auto GetHeight() const { return m_size.height; }

		//				MANIPULATORS
		//

		void SetPosition(const Point2dF &pos)
		{
			m_pos = pos;
		}

		void SetHeight(float h)
		{
			m_size.height = h;
			
			UpdateCursorSize();
		}

		void SetCursorHeightPercent(float p);
		void SetCursorPositionPercent(float p);

		void Render(Renderer &ren);

	private:
		void UpdateCursorSize();

	private:
		Point2dF	m_pos;
		SizeF		m_size;

		RectF			m_padding;

		D2D1_COLOR_F	m_cursorColor;
		D2D1_COLOR_F	m_bgColor;

		float		m_cursorLocalY;
		SizeF		m_cursorSize;
	};
}