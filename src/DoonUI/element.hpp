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

	bool m_active = true;

	Element() = default;
	Element(Coordinate x, Coordinate y, Dimension w, Dimension h);
	//deletes all child elements, do NOT delete child elements yourself
	~Element();

	/* Call whenever the cursor position or the state of the mouse button changes, will NOT invoke any of the button callback functions, for that, a DNUIevent must be sent
	 * @param pos the mouse's screen position, with {0, 0} denoting the center of the screen
	 * @param pressed whether or not the main selection button is held down, used for animations
	 */
	static void set_mouse_state(DNvec2 pos, bool pressed);
	//returns whether or not any element is hovered
	static bool any_hovered() { return s_anyHovered; }
	//returns whether or not any element has keyboard input focus
	static bool any_input_focus() { return s_anyInputFocus; }

	//calls update() on all child elements and updates the transition, can be overriden
	virtual void update(float dt, DNvec2 parentPos, DNvec2 parentSize);
	//calls render() on all child elements, can be overriden
	virtual void render(float parentAlphaMult);
	//calls handle_event() on all child elements, can be overriden
	virtual void handle_event(Event event);

	//get the position of the element when rendered, in pixels. represents the position of the element's center, (0,0) represents the screen's center
	DNvec2 get_render_pos()  { return m_renderPos;  }
	//get the size of the element when rendered, in pixels
	DNvec2 get_render_size() { return m_renderSize; }
	//returns whether the mouse is hovering over the element
	bool is_hovered();

	//sets the current transition, will begin after delay milliseconds
	void set_transition(Transition transition, float delay);

protected:
	inline static DNvec2 s_mousePos;       //the mouse's current screen position
	inline static bool s_mousePressed;     //whether or not the mouse button is held down
	inline static bool s_mousePressedLast; //whether or not the mouse button was held down last frame

	inline static bool s_anyHovered;    //whether any ui element is hovered, useful for determining if game is in focus
	inline static bool s_anyInputFocus; //whether or not any ui element has input focus

	DNvec2 m_renderPos  = {0.0f, 0.0f}; //the final position of the box's center, in pixels
	DNvec2 m_renderSize = {0.0f, 0.0f}; //the final size of the box, in pixels

	//calculates the render size and stores it in m_renderSize
	void calc_render_size(DNvec2 parentSize);
	//calculates the render position and stores it in m_renderPos
	void calc_render_pos(DNvec2 parentPos, DNvec2 parentSize);
};

}; //namespace dnui

#endif