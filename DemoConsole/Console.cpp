#include "Console.h"
#include "utils.h"

//							HELPER FUNCTIONS AND CLASSES
//
static HRESULT TextLayout_GetTextBoundingBoxSize(IN IDWriteTextLayout *textLayout, OUT SizeF *size)
{
	assert(textLayout != nullptr);
	assert(size != nullptr);

	DWRITE_TEXT_METRICS metrics;
	auto hr = textLayout->GetMetrics(&metrics);
	if (SUCCEEDED(hr)) {
		*size = SizeF{ metrics.width, metrics.height };
	}

	return hr;
}

class ColorSwitcher {
public:
	ColorSwitcher(const std::vector<D2D1_COLOR_F> &colors)
		: m_colors(colors)
	{}

	D2D1_COLOR_F Next()
	{
		auto color = m_colors[m_i];
		
		m_i = (m_i + 1) % m_colors.size();

		return color;
	}

private:
	std::vector<D2D1_COLOR_F>	m_colors;
	size_t						m_i{ 0 };
};



//					class Console
//

Console::Console(
	dbgutils::Interpreter interp, size_t histCapa, size_t outputCapa,
	const RectF &rect,
	const GraphicsContext &graphics,
	const wchar_t *promptStr
)
	: m_console(interp, histCapa, outputCapa)
	, m_promptStr(promptStr)
	, m_graphics(graphics)
	, m_rect(rect)
{
	UpdateAllItems();
}

Console::~Console()
{
	SafeRelease(&m_cmdlineItem.textLayout);

	for (auto &item : m_oldItems) {
		SafeRelease(&item.textLayout);
	}
}

void Console::SetRectangle(const RectF &r)
{
	assert(false && "Not implemented yet");

	if (r == m_rect) {
		return;
	}

	m_rect = r;

	UpdateAllItems();
}

bool Console::HandleChar(wchar_t c)
{
	// Ignore non-printable characters.
	if (!iswprint(c)) {
		return false;
	}

	auto changed = m_console.handle_character(c);
	if (!changed) {
		return false;// do not redraw
	}

	auto bboxChanged = UpdateCmdlineItem();
	if (bboxChanged) {
		UpdateOldItems();
	}
	return true;// redraw
}

bool Console::HandleKey(Key key)
{
	auto changed = m_console.handle_key(key);
	if (!changed) {
		return false;// do not redraw
	}

	switch (key) {
		case VK_RETURN: {
			PostProcessReturnKey();
		}break;

		default: {
			auto bboxChanged = UpdateCmdlineItem();
			if (bboxChanged) {
				UpdateOldItems();
			}
		}break;
	}

	return true;// redraw
}

void Console::PostProcessReturnKey()
{
	PushItem_LatestCmdline();	
	PushItem_LatestOutput();
	RemoveOldItemsIfTooMany(2);// remove at most 2

	UpdateAllItems();
}

void Console::PushItem_LatestCmdline()
{
	ConsoleItem	item;
	item.text = m_promptStr + m_console.last_cmdline();
	m_oldItems.push_front(item);
}

void Console::PushItem_LatestOutput()
{
	ConsoleItem	item;
	item.text = m_console.get_output(0);// 0 means "the most recent output".
	m_oldItems.push_front(item);
}

void Console::RemoveOldItemsIfTooMany(size_t n)
{
	if (m_numUpdatedOldItems + 2 > m_console.output_capacity()) {
		for (int i = 0; !m_oldItems.empty() && i < 2; i++) {
			m_oldItems.pop_back();
		}

#ifdef _DEBUG
		OutputDebugStringA("Removing two old items.\n");
#endif
	}
}



//					ITEM UPDATE
//

void Console::UpdateAllItems()
{
	// IMPORTANT NOTE
	//	Always call UpdateCmdlineItem BEFORE UpdateOldItems.
	UpdateCmdlineItem();
	UpdateOldItems();
}

bool Console::UpdateCmdlineItem()
{
	HRESULT hr = S_OK;

	// Save the height before updating the item.
	// Before returning to the caller, the function will check
	// if it changed.
	auto oldHeight = Height(m_cmdlineItem.bbox);

	// Get the command line text from the dbgutils::Console.
	m_cmdlineItem.text = m_promptStr + m_console.cmdline();

	// Recreate the text layout.
	SafeRelease(&m_cmdlineItem.textLayout);

	hr = m_graphics.dwriteFactory->CreateTextLayout(
		m_cmdlineItem.text.c_str(),
		(UINT32)m_cmdlineItem.text.length(),
		m_graphics.textFormat,
		Width(m_rect), Height(m_rect),
		&m_cmdlineItem.textLayout
	);

	// Update the bounding box.
	SizeF	size;
	hr = TextLayout_GetTextBoundingBoxSize(IN m_cmdlineItem.textLayout, OUT &size);
	m_cmdlineItem.bbox = RectF_FromPointAndSize(TopLeft(m_rect), size);

	auto bboxChanged = oldHeight != Height(m_cmdlineItem.bbox);
	return bboxChanged;
}

void Console::UpdateOldItems()
{
	HRESULT hr = S_OK;

	// A counter for how many items have been updated and hence must be rendered.
	m_numUpdatedOldItems = 0;

	// Compute the destination rectangle for the old items
	// located below the command line rectangle.
	// This rectangle will be shrinked in the loop below each time an item is updated.
	auto dest = m_rect;
	dest.top += Height(m_cmdlineItem.bbox);

	// This is the update loop.
	// For each item, we have to...
	//	(1) recreate their text layout and,
	//	(2) compute their bounding box.
	// Their text does not need to be updated.
	for (auto &item : m_oldItems) {
		// Stop if no more room for the current item.
		if (dest.top >= dest.bottom) {
			break;
		}

		// (1) Recreate the text layout.
		SafeRelease(&item.textLayout);

		hr = m_graphics.dwriteFactory->CreateTextLayout(
			item.text.c_str(),
			(UINT32)item.text.length(),
			m_graphics.textFormat,
			Width(dest), Height(dest),
			&item.textLayout
		);

		// (2) Update the bounding box.
		SizeF size;
		hr = TextLayout_GetTextBoundingBoxSize(IN item.textLayout, OUT &size);
		item.bbox = RectF_FromPointAndSize(TopLeft(dest), size);

		++m_numUpdatedOldItems;

		// This is where we shrink the destination rectangle.
		// Compute the position of the next item positioned below.
		dest.top += Height(item.bbox) + 4;
	}
}

//					DRAWING
//


void Console::Draw(Renderer &ren)
{
	DrawBackground(ren);
	DrawCmdline(ren);
	DrawOldItems(ren);
}

void Console::DrawBackground(Renderer &ren)
{
	ren.SaveBrushColor();

	ren.solidBrush->SetColor(ColorFrom3i(0, 20, 80));
	ren.renderTarget->FillRectangle(m_rect, ren.solidBrush);
	
	ren.RestoreBrushColor();
}

void Console::DrawCmdline(Renderer &ren)
{
	auto color = ColorFrom3i(230, 230, 230);

	DrawCmdlineString(ren, color);
	DrawCaret(ren);
}

void Console::DrawCmdlineString(Renderer &ren, const D2D1_COLOR_F &color)
{
	ren.SaveBrushColor();
	ren.solidBrush->SetColor(ColorFrom3i(230, 230, 230));

	ren.renderTarget->DrawTextLayout(TopLeft(m_cmdlineItem.bbox), m_cmdlineItem.textLayout, ren.solidBrush);

	ren.RestoreBrushColor();
}

void Console::DrawCaret(Renderer &ren)
{
	ren.SaveBrushColor();
	ren.solidBrush->SetColor(ColorFrom3i(230, 230, 230));

	// Map text position index to caret coordinate and hit-test rectangle.
	bool isTrailingHit = false; // Use the leading character edge for simplicity here.
	
	DWRITE_HIT_TEST_METRICS htm;
	float x, y;// caret position
	m_cmdlineItem.textLayout->HitTestTextPosition(
		m_promptStr.length() + m_console.caret(),
		isTrailingHit,
		OUT &x,
		OUT &y,
		OUT &htm
	);

	// Get the caret width; respect user settings.
	DWORD w = 1;
	SystemParametersInfo(SPI_GETCARETWIDTH, 0, OUT &w, 0);

	// Draw a thin rectangle.
	auto r = RectF{
		m_cmdlineItem.bbox.left + x - w / 2u,
		m_cmdlineItem.bbox.top + htm.top,
		m_cmdlineItem.bbox.left + x + (w - w / 2u),
		m_cmdlineItem.bbox.top + htm.top + htm.height
	};
	ren.renderTarget->FillRectangle(&r, ren.solidBrush);

	ren.RestoreBrushColor();
}

void Console::DrawOldItems(Renderer &ren)
{
	HRESULT hr = S_OK;

	ren.SaveBrushColor();

	// We will alternate between two colors when rendering the items.
	auto colors = ColorSwitcher({
		ColorFrom3i(0, 200, 0),
		ColorFrom3i(230, 230, 230)
	});

	// Draw each updated item's text layout.
	for (size_t i = 0; i < m_numUpdatedOldItems; i++) {
		assert(i < m_oldItems.size());

		ren.solidBrush->SetColor(colors.Next());

		const auto &item = m_oldItems[i];
		ren.renderTarget->DrawTextLayout(TopLeft(item.bbox), item.textLayout, ren.solidBrush);
	}

	ren.RestoreBrushColor();

#ifdef _DEBUG
	OutputDebugStringA(std::to_string(m_numUpdatedOldItems).c_str());
	OutputDebugStringA("\n");
#endif
}