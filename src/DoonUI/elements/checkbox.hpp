#ifndef CHECKBOX_H
#define CHECKBOX_H

#include "box.hpp"

namespace dnui
{

//a checkbox that sets a boolean variable and runs code when clicked
class Checkbox : public Box
{
public:
	bool* m_val = nullptr;                                 //the value that the checkbox is set to
	void (*m_button_callback)(int, bool, void*) = nullptr; //the function that gets called when the checkbox is clicked
	int m_callbackID = 0;                                  //an id that gets passed to the callback function, used to differentiate checkboxes that have the same callback func
	void* m_userData = nullptr;                            //user-defined data that gets passed to the callback function

	int m_checkedTex   = -1; //the texture the checkbox has when cheched
	int m_uncheckedTex = -1; //the texture the checkbox has when unchecked

	Transition m_checkTransition;   //the transition that plays when the box is checked
	Transition m_uncheckTransition; //the transition that plays when the box is unchecked

	Checkbox() = default;
	Checkbox(Coordinate x, Coordinate y, Dimension w, Dimension h, 
		bool* val, void (*buttonCallback)(int, bool, void*) = nullptr, 
		int callbackID = 0, void* userData = nullptr, int checkedTex = -1, 
		int uncheckedTex = -1, DNvec4 color = {1.0f, 1.0f, 1.0f, 1.0f}, 
		float cornerRad = 0.0f, float angle = 0.0f, DNvec4 outlineColor = {0.0f, 0.0f, 0.0f, 1.0f}, 
		float outlineThickness = 0.0f, Transition checkTransition = Transition(), 
		Transition uncheckTransition = Transition());

	void update(float dt, DNvec2 parentPos, DNvec2 parentSize);
	void handle_event(Event event);
};

}; //namespace dnui

#endif