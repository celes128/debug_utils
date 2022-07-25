#pragma once

#include "framework.h"
#include <deque>
#include "..\debug_utils\Console.h"
#include "geom.h"
#include "GraphicsContext.h"
#include "Renderer.h"
#include "VTextList.h"
#include "Scroller.h"

struct ConsoleItem {
	// The raw string that is layed out in the layout below.
	std::wstring		text;

	// 
	IDWriteTextLayout	*textLayout{ nullptr };

	// Coordinates of the (smallest) bounding box containing the text.
	// TopLeft(bbox) gives the position of the box on the window.
	RectF				bbox;

	~ConsoleItem()
	{
		SafeRelease(&textLayout);
	}

	// RecreateTextLayout (safely) destroys the text layout and creates a new one
	// based on the current text.
	void RecreateTextLayout(const SizeF &size, GraphicsContext &graphics);

	// UpdateBoundingBox updates the position and size of the bounding box.
	// The position is given as the only argument and
	// the size is derived by the size of the text layout.
	void UpdateBoundingBox(const Point2dF &pos);
};

class Console {
public:
	Console(
		dbgutils::Interpreter interp, size_t histCapa, size_t outputCapa,
		const RectF &rect,
		ID2D1HwndRenderTarget *renderTarget,
		const GraphicsContext &graphics,
		const wchar_t *promptStr = L"> "
	);
	~Console();
	
	//			ACCESSORS
	//

	Point2dF GetPosition() const { return TopLeft(m_rect); }
	auto * GetInterpreter() { return m_console.get_interpreter(); }



	//			MANIPULATORS
	//

	// SetRectangle updates the rectangular area where the console is positioned and rendered.
	void SetRectangle(const RectF &r);

	// RETURN VALUE
	//	Returns true iff the console needs to be redrawn.
	bool HandleChar(wchar_t c);

	// RETURN VALUE
	//	Returns true iff the console needs to be redrawn.
	bool HandleKey(Key key);

	// RETURN VALUE
	//	Returns true iff the console needs to be redrawn.
	bool HandleMouseWheel(int mvt);

	void Draw(Renderer &ren);

private:
	//			Construction
	//

	HRESULT CreateRenderTarget(IN ID2D1HwndRenderTarget *renderTarget);
	HRESULT CreateBrush();
	void CreateScroller();

	//			Input handling
	//
	
	void PostProcessReturnKey();

	// RemoveOldItemsIfTooMany removes at most the n oldest item, if there are too
	// many old items in the list.
	void RemoveOldItemsIfTooMany(size_t n);


	//			Layout
	//
	Point2dF GetOutputAreaPosition() const;
	SizeF GetOutputAreaSize() const;


	//			Items view
	//
	
	// MoveItemsView moves the view inside the items list.
	bool MoveItemsView(int mvt);

	//			Item update
	//

	// UpdateAllItems updates both...
	//	(1) the command line item and,
	//	(2) the old items.
	void UpdateAllItems();

	// RETURN VALUE
	//	Returns true iff the height of the command line changed,
	//	meaning the caller should then call UpdateOldItems.
	//
	// SAMPLE CODE
	//
	//	auto bboxChanged = UpdateCmdlineItem();
	//	if (bboxChanged) {
	//		UpdateOldItems();
	//	}
	//
	bool UpdateCmdlineItem();

	//			Drawing
	//

	Renderer GetRenderer();
	void DrawBackground();
	void DrawCmdline();
	void DrawCmdlineRect();
	void DrawCmdlineString(const D2D1_COLOR_F &color);
	void DrawCaret();
	void DrawOldItems();

	void DrawOnMyRenderTarget();
	
	// CopyMyRenderTargetToClient copies the console's render target content into the client's render target.
	//
	// INPUT
	//	Renderer &ren
	//		The client's renderer.
	void CopyMyRenderTargetToClient(Renderer &ren);

private:
	// Logic
	dbgutils::Console			m_console;
	
	// Layout and graphics
	GraphicsContext				m_graphics;
	ID2D1BitmapRenderTarget		*m_renderTarget{ nullptr };
	ID2D1SolidColorBrush		*m_solidBrush{ nullptr };

	std::wstring		m_promptStr;
	RectF				m_rect;
	ConsoleItem			m_cmdlineItem;
	gui::VTextList		m_oldItemsList;

	// A (rectangular) view inside the VTextList m_oldItemsList.
	// Vertical coordinate of the view in the VTextList rectangle.
	float				m_itemsViewY;
	gui::Scroller		m_scroller;
};