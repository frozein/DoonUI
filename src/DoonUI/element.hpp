#ifndef DNUI_ELEMENT_H
#define DNUI_ELEMENT_H

#include <vector>
#include "QuickMath/quickmath.h"
#include "utility.hpp"
#include "render.h"

namespace dnui
{

//a generic UI element, has no functionality on its own other than to update/render child elements
class Element
{
public:
	std::vector<Element*> m_children;

	bool m_activeTransition = false;
	Transition m_transition;

	Coordinate m_xPos;
	Coordinate m_yPos;
	Dimension m_width;
	Dimension m_height;
	float m_alphaMult = 1.0f;

	Element() = default;
	Element(Coordinate x, Coordinate y, Dimension w, Dimension h);
	//deletes all child elements, do NOT delete child elements yourself
	~Element();

	//calls update() on all child elements and updates the transition, can be overriden
	virtual void update(float dt, DNvec2 parentPos, DNvec2 parentSize);
	//calls render() on all child elements, can be overriden
	virtual void render(float parentAlphaMult);
	//calls handle_event() on all child elements, can be overriden
	virtual void handle_event(Event event);

	//sets the current transition, will begin after delay milliseconds
	void set_transition(Transition transition, float delay);

protected:
	DNvec2 m_renderPos;  //the final position of the box's center, in pixels
	DNvec2 m_renderSize; //the final size of the box, in pixels

	//calculates the render size and stores it in m_renderSize
	void calc_render_size(DNvec2 parentSize);
	//calculates the render position and stores it in m_renderPos
	void calc_render_pos(DNvec2 parentPos, DNvec2 parentSize);
};

}; //namespace dnui

#endif