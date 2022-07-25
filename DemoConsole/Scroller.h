#pragma once

namespace gui {

	//	class:		Scroller
	//
	//	The Scroller classes handles logic for vertical or horizontal scrolling.
	//	Scrolling involves to elements: the space and the view.
	//
	//                      <---view--->
	//	<------------------|------------|------------>
	//
	//  The longer bar is the space.
	//	The short one is the view in the space.
	//
	//	The ScrollUp and ScrollDown functions update the position of the view in the space.

	class Scroller {
	public:
		Scroller(float spaceLength, float viewLength, float viewPosPercent = 0.f);


		//					ACCESSORS
		//
		
		float GetViewPositionPercentage() const;


		//					MANIPULATORS
		//

		// ScrollUp and ScrollDown try to move the view.
		//
		// REMARKS
		//	If displacement <= 0.f then the functions do nothing.
		//
		// RETURN VALUE
		//	Returns the position of the view in the space.
		float ScrollUp(float displacement, bool *moved = nullptr);
		float ScrollDown(float displacement, bool *moved = nullptr);

		// REMARKS
		//	If the length is shorter than the current one, then the position
		// of the view might change so that it stays in bounds.
		void SetSpaceLength(float length);

	private:
		float	m_spaceLength;
		float	m_viewLength;
		float	m_viewPosition{ 0.f };
	};
}