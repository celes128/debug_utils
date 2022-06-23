#pragma once

#include "framework.h"
#include "..\debug_utils\Console.h"

class App {
public:
	App();
	~App();

	HRESULT Initialize();

	void RunMessageLoop();

private:
	HRESULT CreateDeviceIndependentResources();
	HRESULT CreateDeviceResources();
	void DiscardDeviceResources();
	void ReleaseResources();

	// Windows message handling
	void on_wm_char(WPARAM wParam);
	void on_wm_keydown(WPARAM wParam);
	void on_resize(const D2D1_SIZE_U &size);

	// Rendering
	HRESULT on_render();
	void clear_window(const D2D1::ColorF &color);
	void draw_editbox();
	void request_redraw();

	D2D1_SIZE_F WindowSizeF() const;
	D2D1_SIZE_U WindowSizeU() const;

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	static const float kFontSize;

private:
	HWND						m_hwnd;
	ID2D1Factory				*m_pD2DFactory;
	IWICImagingFactory			*m_pWICFactory;
	IDWriteFactory				*m_pDWriteFactory;
	ID2D1HwndRenderTarget		*m_pRenderTarget;
	IDWriteTextFormat			*m_pTextFormat;
	ID2D1SolidColorBrush		*m_pSolidBrush;
	ID2D1StrokeStyle			*m_pStrokeStyle;

	dbgutils::Console			m_console;
};