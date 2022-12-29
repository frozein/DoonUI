#ifndef DNUI_ELEMENT_H
#define DNUI_ELEMENT_H

#include <string>
#include <vector>
#include "QuickMath/quickmath.h"
#include "utility.hpp"
#include "render.h"

//--------------------------------------------------------------------------------------------------------------------------------//
//ELEMENT CLASS:

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

//--------------------------------------------------------------------------------------------------------------------------------//
//DEFAULT ELEMENTS:

//a simple rectangle
class Box : public Element
{
public:
	int m_texture = -1;                               //the openGL texture handle to use when rendering, or -1 if no texture is desired
	DNvec4 m_color = {1.0f, 1.0f, 1.0f, 1.0f};        //the box's color
	float m_cornerRadius = 0.0f;                      //the radius of the box's corners, in pixels
	float m_angle = 0.0f;                             //the box's rotation, in degrees
	DNvec4 m_outlineColor = {0.0f, 0.0f, 0.0f, 1.0f}; //the box's outline color
	float m_outlineThickness = 0.0f;                  //the box's outline thickness, in pixels

	Box() = default;
	Box(Coordinate x, Coordinate y, Dimension w, Dimension h, 
		int tex = -1, DNvec4 col = {1.0f, 1.0f, 1.0f, 1.0f}, 
		float cornerRad = 0.0f, float angle = 0.0f, 
		DNvec4 outlineColor = {0.0f, 0.0f, 0.0f, 1.0f}, float outlineThickness = 0.0f);

	void render(float parentAlphaMult);
};

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

//a string of text
class Text : public Element
{
private:
	float m_renderScale; //the final scale of text
	float m_renderW;     //the final maximum line width of the text, in pixels

public:
	//text parameters:
	std::string m_text;                        //the text to render
	DNUIfont* m_font = nullptr;                //the handle to the font to render with
	DNvec4 m_color = {1.0f, 1.0f, 1.0f, 1.0f}; //the text's color
	float m_scale = 1.0f;                      //the scale of the text, if less than or equal to zero, text will scale automatically
	float m_lineWrap = 0.0f;                   //the maximum number of pixels the text can extend before wrapping, set to 0.0 if no wrapping is desired
	int m_align = 0;                           //how to align the text when wrapping: 0 = align left, 1 = align right, 2 = align center
	float m_thickness = 0.5f;                  //the thickness of the text
	float m_softness = 0.05f;                  //the softness of the text's edges

	//outline parameters:
	DNvec4 m_outlineColor = {0.0f, 0.0f, 0.0f, 1.0f}; //the color of the text's outline
	float m_outlineThickness = 1.0f;                  //the thickness at which the text's outline begins
	float m_outlineSoftness = 0.05f;                  //the softness of the outline's edges

	Text() = default;
	Text(Coordinate x, Coordinate y, Dimension size, 
		std::string text, DNUIfont* font, DNvec4 color = {1.0f, 1.0f, 1.0f, 1.0f}, 
		float scale = 0.0f, float lineWrap = 0.0f, int align = 0, 
		float thickness = 0.5f, float softness = 0.05f, 
		DNvec4 outlineColor = {0.0f, 0.0f, 0.0f, 0.0f}, float outlineThickness = 1.0f, 
		float outlineSoftness = 0.05f);

	void update(float dt, DNvec2 parentPos, DNvec2 parentSize);
	void render(float parentAlphaMult);
};


}; //namespace dnui

#endif