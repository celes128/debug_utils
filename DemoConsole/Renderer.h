#pragma once

#include "framework.h"

struct Renderer {
	ID2D1HwndRenderTarget	*renderTarget{ nullptr };
	ID2D1SolidColorBrush	*solidBrush{ nullptr };
	D2D1_COLOR_F			savedBrushColor{ 0 };

	void SaveBrushColor()
	{
		savedBrushColor = solidBrush->GetColor();
	}

	void RestoreBrushColor()
	{
		solidBrush->SetColor(savedBrushColor);
	}
};