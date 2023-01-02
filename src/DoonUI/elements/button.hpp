#ifndef BUTTON_H
#define BUTTON_H

#include "box.hpp"

namespace dnui
{

//a button that calls a user-defined function when clicked
class Button : public Box
{
public:
	void (*m_button_callback)(int) = nullptr; //the function that gets called when the button is clicked
	int m_callbackID = 0;                     //the unique id that gets passed to the callback function, used to differentiate buttons that have the same callback func

	Transition m_baseTransition;  //the transition that plays when the button is neither hovered nor held down
	Transition m_hoverTransition; //the transition that plays when the button is hovered
	Transition m_holdTransition;  //the transition that plays when the button is held down

	Button() = default;
	Button(Coordinate x, Coordinate y, Dimension w, Dimension h, 
		void (*buttonCallback)(int) = nullptr, int callbackID = 0, 
		int tex = -1, DNvec4 color = {1.0f, 1.0f, 1.0f, 1.0f}, 
		float cornerRad = 0.0f, float angle = 0.0f, 
		DNvec4 outlineColor = {0.0f, 0.0f, 0.0f, 1.0f}, float outlineThickness = 0.0f, 
		Transition base = Transition(), Transition hover = Transition(), Transition hold = Transition());

	/* Call whenever the cursor position or the state of the mouse button changes, will NOT invoke any of the button callback functions, for that, a DNUIevent must be sent
	 * @param pos the mouse's screen position, with {0, 0} denoting the center of the screen
	 * @param pressed whether or not the main selection button is held down, used for animations
	 */
	static void set_mouse_state(DNvec2 pos, bool pressed);
	/* Call whenever you change any of the buttons transitions in order to have them refresh immediately,
	 * or they may take a while to show
	*/
	void update_transitions();

	void update(float dt, DNvec2 parentPos, DNvec2 parentSize);
	void handle_event(Event event);

private:
	inline static DNvec2 s_mousePos;   //the mouse's current screen position
	inline static bool s_mousePressed; //whether or not the mouse button is held down

	int m_curState = 0; //0 = not hovered, 1 = hovered, 2 = held
};

}; //namespace dnui

#endif