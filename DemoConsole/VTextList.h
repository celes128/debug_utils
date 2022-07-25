#pragma once

#include "framework.h"
#include <deque>
#include <string>
#include "geom.h"
#include "GraphicsContext.h"
#include "Renderer.h"

namespace gui {

	struct TextItem {
		GraphicsContext		graphics;

		// The raw string that is layed out in the layout below.
		std::wstring		text;

		D2D1_COLOR_F		textColor{ D2D1::ColorF(D2D1::ColorF::White) };
		D2D1_COLOR_F		bgColor{ D2D1::ColorF(D2D1::ColorF::Black) };

		// 
		IDWriteTextLayout	*textLayout{ nullptr };

		// Coordinates of the (smallest) bounding box containing the text.
		// TopLeft(bbox) gives the position of the box on the window.
		RectF				bbox;

		TextItem() = default;
		TextItem(const TextItem& other);
		
		TextItem& operator=(const TextItem& other) = delete;

		// Move assignment operator.
		TextItem& operator=(TextItem&& other) noexcept
		{
			if (this != &other) {
				// Free the existing resource.
				SafeRelease(&textLayout);

				// Copy the data pointer and its length from the source object.
				graphics = other.graphics;
				text = other.text;
				textColor = other.textColor;
				bgColor = other.bgColor;
				textLayout = other.textLayout;
				bbox = other.bbox;

				// Release the data pointer from the source object so that
				// the destructor does not free the memory multiple times.
				other.textLayout = nullptr;
			}

			return *this;
		}

		~TextItem()
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



	//	class:				VTextList
	//
	//	The VTextList is a container of text items laid out vertically.
	//	The class manages the positioning and rendering of the text items
	//	inside a virtual rectangle whose width is fixed and height changes
	//	dynamically as items are added or removed.

	class VTextList {
	public:
		VTextList(
			const GraphicsContext &graphics,
			float width);

		//				ACCESSORS
		//

		float GetWidth() const { return m_width; }
		float GetHeight() const;
		D2D1_SIZE_F GetSize() const { return { GetWidth(), GetHeight() }; }

		size_t NumItems() const { return m_items.size(); }

		//				MANIPULATORS
		//

		void SetWidth(float w);

		// PushBack adds an item at the back of the list.
		void PushBack(
			const std::wstring &text,
			const D2D1_COLOR_F &textColor = D2D1::ColorF(D2D1::ColorF::White),
			const D2D1_COLOR_F &bgColor = D2D1::ColorF(D2D1::ColorF::Black));

		// PopFront removes n items from the front of the list.
		// RETURN VALUE
		//	Returns the number of items removed.
		size_t PopFront(size_t n = 1);

		//// Iterators.
		//auto begin() const { return m_items.cbegin(); }
		//auto end() const { return m_items.end(); }

		// DrawView draws the subset of items that overlap a rectangle (the view).
		void DrawView(const RectF &view, const Point2dF &pos, Renderer ren);

		struct Range {
			size_t	begin{ 0 };
			size_t	end{ 0 };

			bool Empty() const { return begin >= end; }

			static Range MakeEmpty() { return { 0,0 }; }
		};

		// GetItemsInView determines the range of text items that overlap a rectangular view.
		Range GetItemsInView(const RectF &view);

		// RETURN VALUE
		//	Returns true iff an item crossed by the line was found.
		//	The item index is written to k.
		bool FindItemIntersectingHorizLine(float y, size_t *k);

	private:
		void RecreateAllItems();
		void UpdateAllItemsPositions();

	private:
		GraphicsContext			m_graphics;
		float					m_width{ 0.f };
		std::deque<TextItem>	m_items;
	};
}