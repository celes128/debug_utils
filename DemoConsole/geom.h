#pragma once

#include <d2d1.h>
//#include <d2d1helper.h>

using Point2dF	= D2D1_POINT_2F;
using SizeF		= D2D1_SIZE_F;
using RectF		= D2D1_RECT_F;

inline Point2dF Point2dF_Zero()
{
	return { 0.f,0.f };
}

inline Point2dF operator+(const Point2dF &left, const Point2dF &right)
{
	return {
		left.x + right.x,
		left.y + right.y
	};
}

//			Rectangle construction
//
inline RectF	RectF_Null() { return RectF{ 0.F,0.F,0.F,0.F }; }

inline RectF	RectF_FromPointAndSize(const Point2dF &p, const SizeF &size)
{
	return RectF{
		p.x,
		p.y,
		p.x + size.width,
		p.y + size.height
	};
};

//			Rectangle accessors
//
inline float	Width(const RectF &r) { return r.right - r.left; }
inline float	Height(const RectF &r) { return r.bottom - r.top; }
inline SizeF	Size(const RectF &r) { return { Width(r), Height(r)}; }
inline Point2dF TopLeft(const RectF &r) { return { r.left, r.top }; }

inline bool operator==(const RectF &lhs, const RectF &rhs)
{
	return lhs.left == rhs.left
		&& lhs.right == rhs.right
		&& lhs.top == rhs.top
		&& lhs.bottom == rhs.bottom;
}

inline bool operator!=(const RectF &lhs, const RectF &rhs)
{
	return !(lhs == rhs);
}

inline bool RectRectIntersect(const RectF &a, const RectF &b)
{
	if (a.right < b.left || b.right < a.left) return false;
	if (a.bottom < b.top || b.bottom < a.top) return false;
	return true;
}