#include "VTextList.h"
#include <cassert>

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

namespace gui {

	void TextItem::RecreateTextLayout(const SizeF &size, GraphicsContext &graphics)
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

	void TextItem::UpdateBoundingBox(const Point2dF &pos)
	{
		SizeF	size;
		auto hr = TextLayout_GetTextBoundingBoxSize(IN textLayout, OUT &size);

		bbox = RectF_FromPointAndSize(pos, size);
	}


	static const float VeryLargeHeight = 99999.f;

	VTextList::VTextList(const GraphicsContext &graphics, float width, VTEXTLIST_DRAW_ORDER drawOrder)
		: m_graphics(graphics)
		, m_width(width)
		, m_drawOrder(drawOrder)
	{
		assert(width >= 1.f);
	}

	float VTextList::GetHeight() const
	{
		auto h = 0.f;

		for (const auto &item : m_items) {
			h += Height(item.bbox);
		}

		return h;
	}

	void VTextList::SetWidth(float w)
	{
		assert(w >= 1.f);

		m_width = w;

		RecreateAllItems();
	}

	void VTextList::PushBack(
		const std::wstring &text,
		const D2D1_COLOR_F &textColor,
		const D2D1_COLOR_F &bgColor)
	{
		TextItem item;
		item.text = text;
		item.textColor = textColor;
		item.bgColor = bgColor;
		item.RecreateTextLayout(D2D1_SIZE_F{ m_width, VeryLargeHeight }, m_graphics);
		item.UpdateBoundingBox({ 0.f, GetHeight() });

		m_items.push_back(item);
	}

	void VTextList::PopFront(size_t n)
	{
		for (size_t i = 0; i < n && !m_items.empty(); i++) {
			m_items.pop_front();
		}
	}

	void VTextList::RecreateAllItems()
	{
		auto y = 0.f;

		for (auto &item : m_items) {
			item.RecreateTextLayout(D2D1_SIZE_F{ m_width, VeryLargeHeight }, m_graphics);

			item.UpdateBoundingBox({ 0.f, y });

			y += Height(item.bbox);
		}
	}

	void VTextList::UpdateAllItemsPositions()
	{
		auto y = 0.f;

		for (auto &item : m_items) {
			item.UpdateBoundingBox({ 0.f, y });

			y += Height(item.bbox);
		}
	}

	void VTextList::DrawView(
		const RectF &view,
		const Point2dF &pos,
		Renderer &ren)
	{
		ren.SaveBrushColor();

		for (const auto &item : m_items) {
			// Draw the background rectangle.
			ren.solidBrush->SetColor(item.bgColor);
			auto bgRect = RectF_FromPointAndSize(pos + TopLeft(item.bbox), Size(item.bbox));
			ren.renderTarget->FillRectangle(bgRect, ren.solidBrush);

			// Draw the text.
			ren.solidBrush->SetColor(item.textColor);
			ren.renderTarget->DrawTextLayout(pos + TopLeft(item.bbox), item.textLayout, ren.solidBrush);
		}

		ren.RestoreBrushColor();
	}
}