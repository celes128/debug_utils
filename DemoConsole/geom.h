#pragma once

#include <d2d1.h>
//#include <d2d1helper.h>

using Point2dF	= D2D1_POINT_2F;
using SizeF		= D2D1_SIZE_F;
using RectF		= D2D1_RECT_F;

inline RectF RectF_FromPointAndSize(const Point2dF &p, const SizeF &size)
{
	return RectF{
		p.x, p.y,
		size.width, size.height
	};
};

inline float Width(const RectF &r) { return r.right - r.left; }
inline float Height(const RectF &r) { return r.bottom - r.top; }
inline Point2dF TopLeft(const RectF &r) { return { r.left, r.top }; }