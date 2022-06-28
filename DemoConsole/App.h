#pragma once

#include "framework.h"
#include <deque>
#include "..\debug_utils\Console.h"
#include "..\debug_utils\string_utils.h"
#include "geom.h"
#include "Console.h"

class App {
public:
	App();
	~App();

	HRESULT Initialize();

	void RunMessageLoop();

private:
	//		Initialization and shutdown

	HRESULT CreateDeviceIndependentResources();
	HRESULT CreateDeviceResources();
	void DiscardDeviceResources();
	void ReleaseResources();

	//		Windows message handling

	void OnWMChar(WPARAM wParam);
	void OnWMKeydown(WPARAM wParam);
	void OnResize(const D2D1_SIZE_U &size);

	//		Console
	void CreateTheConsole();

	// RETURN VALUE
	// Returns true iff the height of the command line changed, meaning we should
	// call UpdateOldItems.
	bool UpdateCmdlineItem();

	void UpdateOldItems();

	//		Rendering

	HRESULT on_render();
	void clear_window(const D2D1::ColorF &color);
	void draw_console();
	void draw_cmdline(const RectF &dest);
	void draw_caret();
	void draw_old_items(const RectF &dest);
	void request_redraw();

	// Utils

	D2D1_SIZE_F		WindowSizeF() const;
	D2D1_SIZE_U		WindowSizeU() const;
	RectF			WindowRectF() const;
	GraphicsContext GetGraphicsContext() const;

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	static const float kFontSize;

private:
	HWND					m_hwnd;
	ID2D1Factory			*m_pD2DFactory;
	IWICImagingFactory		*m_pWICFactory;
	IDWriteFactory			*m_pDWriteFactory;
	ID2D1HwndRenderTarget	*m_pRenderTarget;
	IDWriteTextFormat		*m_pTextFormat;
	ID2D1SolidColorBrush	*m_pSolidBrush;
	ID2D1StrokeStyle		*m_pStrokeStyle;

	Console					*m_console;
};