#pragma once

inline D2D1::ColorF ColorFrom3i(int red, int green, int blue, int alpha = 255)
{
	return D2D1::ColorF(red / 255.f, green / 255.f, blue / 255.f, alpha / 255.f);
}

inline bool is_key_down(WPARAM wParam)
{
	return GetAsyncKeyState((int)wParam) & 0x8000;
}