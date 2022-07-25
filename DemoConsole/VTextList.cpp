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

	static const float VeryLargeHeight = 99999.f;

	TextItem::TextItem(const TextItem& other)
	{
		// Copy the data pointer and its length from the source object.
		graphics = other.graphics;
		text = other.text;
		textColor = other.textColor;
		bgColor = other.bgColor;
		bbox = other.bbox;

		RecreateTextLayout(D2D1_SIZE_F{ Width(other.bbox), VeryLargeHeight }, graphics);
	}

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



	VTextList::VTextList(const GraphicsContext &graphics, float width)
		: m_graphics(graphics)
		, m_width(width)
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
		item.graphics = m_graphics;
		item.text = text;
		item.textColor = textColor;
		item.bgColor = bgColor;
		item.RecreateTextLayout(D2D1_SIZE_F{ m_width, VeryLargeHeight }, m_graphics);
		item.UpdateBoundingBox({ 0.f, GetHeight() });// dummy position

		m_items.push_back(std::move(item));

		UpdateAllItemsPositions();
	}

	size_t VTextList::PopFront(size_t n)
	{
		size_t i = 0;
		for (; i < n && !m_items.empty(); i++) {
			m_items.pop_front();
		}
		return i;
	}

	void VTextList::RecreateAllItems()
	{
		auto y = 0.f;

		for (auto it = m_items.rbegin(); it != m_items.rend(); it++) {
		//for (auto it = m_items.begin(); it != m_items.end(); it++) {
			it->RecreateTextLayout(D2D1_SIZE_F{ m_width, VeryLargeHeight }, m_graphics);

			it->UpdateBoundingBox({ 0.f, y });

			y += Height(it->bbox);
		}
	}

	void VTextList::UpdateAllItemsPositions()
	{
		auto y = 0.f;

		for (auto it = m_items.rbegin(); it != m_items.rend(); it++) {
		//for (auto it = m_items.begin(); it != m_items.end(); it++) {
			it->UpdateBoundingBox({ 0.f, y });

			y += Height(it->bbox);
		}
	}

	void VTextList::DrawView(const RectF &view, const Point2dF &pos, Renderer ren)
	{
		auto range = GetItemsInView(view);

		//// DEBUG
		//OutputDebugStringA(std::to_string(range.begin).c_str());
		//OutputDebugStringA(",");
		//OutputDebugStringA(std::to_string(range.end).c_str());
		//OutputDebugStringA("\n");

		if (range.Empty()) {
			return;
		}
		
		assert(0 <= range.begin && range.begin < NumItems());
		assert(0 <= range.end && range.end <= NumItems());

		auto yOffset = - view.top;
		//// DEBUG
		//OutputDebugStringA(std::to_string(yOffset).c_str());
		//OutputDebugStringA(";  ");

		for (auto i = range.begin; i < range.end; i++) {
			const auto &item = m_items[i];

			// Position on the render target.
			auto p = pos + TopLeft(item.bbox) + Point2dF{0.f, yOffset};

			// Draw the background rectangle.
			auto size = SizeF{ GetWidth(), Height(item.bbox) };
			auto rect = RectF_FromPointAndSize(p, size);

			ren.solidBrush->SetColor(item.bgColor);
			ren.renderTarget->FillRectangle(rect, ren.solidBrush);

			// Draw the text.
			ren.solidBrush->SetColor(item.textColor);
			ren.renderTarget->DrawTextLayout(p, item.textLayout, ren.solidBrush);
		}
	}

	VTextList::Range VTextList::GetItemsInView(const RectF &view)
	{
		// DEBUG
		const auto n = NumItems();

		// Find the item that overlaps the top of the view.
		size_t i;
		auto found = FindItemIntersectingHorizLine(view.top, &i);
		size_t end = found ? i+1 : m_items.size();

		// Find the item that overlaps the bottom of the view.
		size_t begin;
		found = FindItemIntersectingHorizLine(view.bottom, &begin);
		if (!found) {
			begin = 0;
		}

		return Range{ begin, end };
	}

	bool VTextList::FindItemIntersectingHorizLine(float y, size_t *k)
	{
		for (size_t i = 0; i < m_items.size(); i++) {
			const auto &box = m_items[i].bbox;
			if (box.top <= y && y <= box.bottom) {
				if (*k) {
					*k = i;
				}
				return true;
			}
		}

		return false;
	}
}