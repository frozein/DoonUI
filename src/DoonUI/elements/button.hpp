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

	/* Call whenever you change any of the buttons transitions in order to have them refresh immediately,
	 * or they may take a while to show
	*/
	void update_transitions();

	void update(float dt, DNvec2 parentPos, DNvec2 parentSize);
	void handle_event(Event event);

protected:
	int m_curState = 0; //0 = not hovered, 1 = hovered, 2 = held
};

}; //namespace dnui

#endif