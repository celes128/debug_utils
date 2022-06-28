#pragma once

#include "framework.h"
#include <deque>
#include "..\debug_utils\Console.h"
#include "geom.h"
#include "GraphicsContext.h"
#include "Renderer.h"

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
};

class Console {
public:
	Console(dbgutils::Interpreter interp, size_t histCapa, size_t outputCapa, const RectF &rect, const GraphicsContext &graphics);
	~Console();
	
	//			MANIPULATORS
	//

	// SetRectangle updates the rectangular area where the console is positioned and rendered.
	void SetRectangle(const RectF &r);

	bool HandleChar(wchar_t c);
	
	bool HandleKey(Key key);

	void Draw(Renderer &ren);

private:
	//			Input handling
	//
	void PostProcessReturnKey();


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

	void UpdateOldItems();

	//			Drawing
	//

	void DrawCmdline(Renderer &ren);
	void DrawOldItems(Renderer &ren);

private:
	// Logic
	dbgutils::Console			m_console;
	
	// Layout + Graphics
	GraphicsContext				m_graphics;
	RectF						m_rect;
	ConsoleItem					m_cmdlineItem;
	std::deque<ConsoleItem>		m_oldItems;
	size_t						m_numUpdatedOldItems{ 0 };
};