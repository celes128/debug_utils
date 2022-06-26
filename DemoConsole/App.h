#pragma once

#include "framework.h"
#include "..\debug_utils\Console.h"
#include "..\debug_utils\string_utils.h"
#include "geom.h"
#include <deque>

struct ConsoleItem {
	// The raw string that is layed out in the layout below.
	std::wstring		text;

	// 
	IDWriteTextLayout	*textLayout{ nullptr };
	
	// Coordinates of the (smallest) bounding box containing the text.
	// TopLeft(bbox) gives the position of the box on the window.
	RectF				bbox;
};

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

	// UpdateConsoleItems (re)computes the console items, i.e.
	// both the command line one and the old ones, so that
	// they become ready to be rendered.
	void UpdateConsoleItems();
		void UpdateCmdlineItem();
		void UpdateOldItems();

	// Rendering
	HRESULT on_render();
	void clear_window(const D2D1::ColorF &color);
	void draw_console();
	void draw_cmdline(const RectF &dest);
	void draw_caret();
	void draw_old_items(const RectF &dest);
	void request_redraw();

	D2D1_SIZE_F WindowSizeF() const;
	D2D1_SIZE_U WindowSizeU() const;
	RectF		WindowRectF() const;

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

	// CONSOLE
	//
	// Logic
	dbgutils::Console			m_console;
	// Layout + Graphics
	RectF						m_consoleDestRect;
	ConsoleItem					m_cmdlineItem;
	std::deque<ConsoleItem>		m_oldItems;
	size_t						m_numUpdatedOldItems{ 0 };
};