#include "Console.h"
#include "utils.h"
#include <stdexcept>

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

void ConsoleItem::RecreateTextLayout(const SizeF &size, GraphicsContext &graphics)
{
	SafeRelease(&textLayout);

	auto hr = graphics.dwriteFactory->CreateTextLayout(
		text.c_str(),
		(UINT32)text.length(),
		graphics.textFormat,
		size.width, size.height,
		&textLayout
	);
}

void ConsoleItem::UpdateBoundingBox(const Point2dF &pos)
{
	SizeF	size;
	auto hr = TextLayout_GetTextBoundingBoxSize(IN textLayout, OUT &size);
	
	bbox = RectF_FromPointAndSize(pos, size);
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



//		class:				Console
//

Console::Console(
	dbgutils::Interpreter interp, size_t histCapa, size_t outputCapa,
	const RectF &rect,
	ID2D1HwndRenderTarget *renderTarget,
	const GraphicsContext &graphics,
	const wchar_t *promptStr
)
	: m_console(interp, histCapa, outputCapa)
	, m_promptStr(promptStr)
	, m_graphics(graphics)
	, m_rect(rect)
	, m_oldItemsList(graphics, Width(rect))
{
	assert(renderTarget != nullptr);

	HRESULT hr = S_OK;
	
	hr = CreateRenderTarget(renderTarget);
	if (FAILED(hr)) {
		throw std::runtime_error("Console::Console(...) failed to create its render target.");
	}

	hr = CreateBrush();
	if (FAILED(hr)) {
		throw std::runtime_error("Console::Console(...) failed to create its brush.");
	}

	UpdateAllItems();
}

HRESULT Console::CreateRenderTarget(IN ID2D1HwndRenderTarget *renderTarget)
{
	assert(renderTarget != nullptr);

	auto size = D2D1_SIZE_U{ (UINT32)Width(m_rect), (UINT32)Height(m_rect) };
	auto pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_UNKNOWN);

	auto hr = renderTarget->CreateCompatibleRenderTarget(
		NULL,
		&size,
		&pixelFormat,
		D2D1_COMPATIBLE_RENDER_TARGET_OPTIONS_NONE,
		&m_renderTarget);

	return hr;
}

HRESULT Console::CreateBrush()
{
	assert(m_renderTarget != nullptr);

	HRESULT hr = S_OK;

	hr = m_renderTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Red),
		&m_solidBrush);

	return hr;
}

Console::~Console()
{
	SafeRelease(&m_cmdlineItem.textLayout);
	SafeRelease(&m_solidBrush);
	SafeRelease(&m_renderTarget);
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
		//UpdateOldItems();
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
				//UpdateOldItems();
			}
		}break;
	}

	return true;// redraw
}

void Console::PostProcessReturnKey()
{
	// Push the command line that has just got processed.
	m_oldItemsList.PushBack(
		m_promptStr + m_console.last_cmdline(),
		ColorFrom3i(255, 130, 36),
		D2D1::ColorF(D2D1::ColorF::Black));

	// Push the output that was generated.
	m_oldItemsList.PushBack(
		m_console.get_output(0),// 0 means "the most recent output"
		D2D1::ColorF(D2D1::ColorF::Black),
		D2D1::ColorF(D2D1::ColorF::White));
	
	UpdateAllItems();

	// Remove at most 2 items.
	RemoveOldItemsIfTooMany(2);
}

void Console::RemoveOldItemsIfTooMany(size_t n)
{
	bool tooMany = m_oldItemsList.NumItems() > m_console.output_capacity();
	if (!tooMany) {
		return;
	}

	auto numRemoved = m_oldItemsList.PopFront(n);

	#ifdef _DEBUG
	OutputDebugStringA(std::to_string(numRemoved).c_str());
	OutputDebugStringA(" items got removed.\n");
	#endif
}


//			Layout
//

Point2dF Console::GetOutputAreaPosition() const
{
	auto x = 0.f;
	auto y = Height(m_cmdlineItem.bbox);

	return { x,y };
}

SizeF Console::GetOutputAreaSize() const
{
	auto w = Width(m_rect);
	auto h = Height(m_rect) - Height(m_cmdlineItem.bbox);

	return { w,h };
}


//					ITEM UPDATE
//

void Console::UpdateAllItems()
{
	// IMPORTANT NOTE
	//	Always call UpdateCmdlineItem BEFORE UpdateOldItems.
	UpdateCmdlineItem();
	//UpdateOldItems();
}

bool Console::UpdateCmdlineItem()
{
	HRESULT hr = S_OK;

	// Save the height before updating the item.
	// Before returning to the caller, the function will check if it changed.
	auto oldHeight = Height(m_cmdlineItem.bbox);

	// Get the command line text from the dbgutils::Console.
	m_cmdlineItem.text = m_promptStr + m_console.cmdline();

	m_cmdlineItem.RecreateTextLayout(Size(m_rect), m_graphics);

	m_cmdlineItem.UpdateBoundingBox(Point2dF_Zero());

	auto bboxChanged = oldHeight != Height(m_cmdlineItem.bbox);
	return bboxChanged;
}

//					DRAWING
//


void Console::Draw(Renderer &ren)
{
	DrawOnMyRenderTarget();
	CopyMyRenderTargetToClient(ren);
}

void Console::DrawOnMyRenderTarget()
{
	// Initialize rendering.
	m_renderTarget->BeginDraw();
	m_renderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	m_renderTarget->Clear(ColorFrom3i(0, 0, 0));

	// Draw each element of the console here...
	DrawBackground();
	DrawCmdline();
	DrawOldItems();

	// Finalize rendering.
	auto hr = m_renderTarget->EndDraw();
	if (hr == D2DERR_RECREATE_TARGET) {
		assert(false && "Error handling not yet implemented.");
	}
}

void Console::CopyMyRenderTargetToClient(Renderer &ren)
{
	ID2D1Bitmap *sourceBitmap = nullptr;
	m_renderTarget->GetBitmap(&sourceBitmap);

	ren.renderTarget->DrawBitmap(
		sourceBitmap,
		m_rect,
		1.f,
		D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
}

Renderer Console::GetRenderer()
{
	return Renderer{
		m_renderTarget,
		m_solidBrush
	};
}

void Console::DrawBackground()
{
	auto ren = GetRenderer();

	ren.SaveBrushColor();

	ren.solidBrush->SetColor(ColorFrom3i(0, 20, 80));
	//ren.solidBrush->SetColor(ColorFrom3i(23, 67, 135));

	auto rect = RectF_FromPointAndSize(Point2dF_Zero(), Size(m_rect));

	ren.renderTarget->FillRectangle(rect, ren.solidBrush);
	
	ren.RestoreBrushColor();
}

void Console::DrawCmdline()
{
	auto color = ColorFrom3i(230, 230, 230);

	DrawCmdlineRect();
	DrawCmdlineString(color);
	DrawCaret();
}

void Console::DrawCmdlineRect()
{
	auto ren = GetRenderer();


	auto size = SizeF{ Width(m_rect), Height(m_cmdlineItem.bbox) };
	auto rect = RectF_FromPointAndSize(Point2dF_Zero(), size);

	ren.solidBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
	ren.renderTarget->FillRectangle(rect, ren.solidBrush);

	ren.solidBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Green));
	ren.renderTarget->DrawRectangle(rect, ren.solidBrush, 4.f);
}

void Console::DrawCmdlineString(const D2D1_COLOR_F &color)
{
	auto ren = GetRenderer();
	
	ren.SaveBrushColor();
	ren.solidBrush->SetColor(ColorFrom3i(230, 230, 230));

	ren.renderTarget->DrawTextLayout(TopLeft(m_cmdlineItem.bbox), m_cmdlineItem.textLayout, ren.solidBrush);

	ren.RestoreBrushColor();
}

void Console::DrawCaret()
{
	auto ren = GetRenderer();
	
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

void Console::DrawOldItems()
{
	HRESULT hr = S_OK;

	//// We will alternate between two colors when rendering the items.
	//auto colors = ColorSwitcher({
	//	//ColorFrom3i(0, 200, 0),
	//	ColorFrom3i(255, 130, 36),
	//	ColorFrom3i(230, 230, 230)
	//});

	auto view = RectF_FromPointAndSize({ 0.f,0.f }, GetOutputAreaSize());
	auto p = GetOutputAreaPosition();
	m_oldItemsList.DrawView(view, p, GetRenderer());
}
