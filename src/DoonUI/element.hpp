#ifndef DNUI_ELEMENT_H
#define DNUI_ELEMENT_H

#include <string>
#include <vector>
#include "math/common.h"
#include "utility.hpp"

//--------------------------------------------------------------------------------------------------------------------------------//
//ELEMENT CLASS:

//a generic UI element, has no functionality on its own other than to update/render child elements
class DNUIelement
{
protected:
	DNvec2 renderPos;  //the final position of the box's center, in pixels
	DNvec2 renderSize; //the final size of the box, in pixels

public:
	std::vector<DNUIelement*> children;

	bool activeTransition = false;
	DNUItransition transition;

	DNUIcoordinate xPos;
	DNUIcoordinate yPos;
	DNUIdimension width;
	DNUIdimension height;
	float alphaMult = 1.0f;

	DNUIelement() = default;
	DNUIelement(DNUIcoordinate x, DNUIcoordinate y, DNUIdimension w, DNUIdimension h);
	//deletes all child elements, do NOT delete child elements yourself
	~DNUIelement();

	//calls update() on all child elements and updates the transition, can be overriden
	virtual void update(float dt, DNvec2 parentPos, DNvec2 parentSize);
	//calls render() on all child elements, can be overriden
	virtual void render(float parentAlphaMult);
	//calls handle_event() on all child elements, can be overriden
	virtual void handle_event(DNUIevent event);

	//sets the current transition, will begin after delay milliseconds
	void set_transition(DNUItransition transition, float delay);
};

//--------------------------------------------------------------------------------------------------------------------------------//
//DEFAULT ELEMENTS:

//a simple rectangle
class DNUIbox : public DNUIelement
{
public:
	int texture = -1;                               //the openGL texture handle to use when rendering, or -1 if no texture is desired
	DNvec4 color = {1.0f, 1.0f, 1.0f, 1.0f};        //the box's color
	float cornerRadius = 0.0f;                      //the radius of the box's corners, in pixels
	float angle = 0.0f;                             //the box's rotation, in degrees
	DNvec4 outlineColor = {0.0f, 0.0f, 0.0f, 1.0f}; //the box's outline color
	float outlineThickness = 0.0f;                  //the box's outline thickness, in pixels

	DNUIbox() = default;
	DNUIbox(DNUIcoordinate x, DNUIcoordinate y, DNUIdimension w, DNUIdimension h, int tex = -1, DNvec4 col = {1.0f, 1.0f, 1.0f, 1.0f}, float cornerRad = 0.0f, float angle = 0.0f, DNvec4 outlineColor = {0.0f, 0.0f, 0.0f, 1.0f}, float outlineThickness = 0.0f);

	void render(float parentAlphaMult);
};

//a button that calls a user-defined function when clicked
class DNUIbutton : public DNUIbox
{
private:
	inline static DNvec2 mousePos;  //the mouse's current screen position
	inline static bool mousePressed; //whether or not the mouse button is held down

	int curState = 0; //0 = not hovered, 1 = hovered, 2 = held

public:
	void (*button_callback)(int) = nullptr; //the function that gets called when the button is clicked
	int callbackID = 0;						//the unique id that gets passed to the callback function, used to differentiate buttons that have the same callback func

	DNUItransition baseTransition;  //the base transition state of the button. NOTE: if the position/size/etc of the button is changed, the base transition should also be changed to reflect this
	DNUItransition hoverTransition; //the transition that plays when the button is hovered
	DNUItransition holdTransition;  //the transition that plays when the button is held down

	DNUIbutton() = default;
	DNUIbutton(DNUIcoordinate x, DNUIcoordinate y, DNUIdimension w, DNUIdimension h, void (*buttonCallback)(int) = nullptr, int callbackID = 0, int tex = -1, DNvec4 color = {1.0f, 1.0f, 1.0f, 1.0f}, float cornerRad = 0.0f, float angle = 0.0f, DNvec4 outlineColor = {0.0f, 0.0f, 0.0f, 1.0f}, float outlineThickness = 0.0f, DNUItransition base = DNUItransition(), DNUItransition hover = DNUItransition(), DNUItransition hold = DNUItransition());

	/* Call whenever the cursor position or the state of the mouse button changes, will NOT invoke any of the button callback functions, for that, a DNUIevent must be sent
	 * @param pos the mouse's screen position, with {0, 0} denoting the center of the screen
	 * @param pressed whether or not the main selection button is held down, used for animations
	 */
	static void set_mouse_state(DNvec2 pos, bool pressed);

	void update(float dt, DNvec2 parentPos, DNvec2 parentSize);
	void handle_event(DNUIevent event);
};

//a string of text
class DNUItext : public DNUIelement
{
private:
	float renderScale; //the final scale of text
	float renderW;	   //the final maximum line width of the text, in pixels

public:
	//text parameters:
	std::string text;						 //the text to render
	int font = -1;							 //the handle to the font to render with
	DNvec4 color = {1.0f, 1.0f, 1.0f, 1.0f}; //the text's color
	float scale = 1.0f;						 //the scale of the text, or -1 for automatic scaling
	float lineWrap = 0.0f;					 //the maximum number of pixels the text can extend before wrapping, to 0.0 if no wrapping is desired
	int align = 0;							 //how to align the text when wrapping: 0 = align left, 1 = align right, 2 = align center
	float thickness = 0.5f;					 //the thickness of the text
	float softness = 0.05f;					 //the softness of the text's edges

	//outline parameters:
	DNvec4 outlineColor = {0.0f, 0.0f, 0.0f, 1.0f}; //the color of the text's outline
	float outlineThickness = 1.0f;					//the thickness at which the text's outline begins
	float outlineSoftness = 0.05f;					//the softness of the outline's edges

	DNUItext() = default;
	DNUItext(DNUIcoordinate x, DNUIcoordinate y, DNUIdimension size, std::string text, int font, DNvec4 color = {1.0f, 1.0f, 1.0f, 1.0f}, float scale = 1.0f, float lineWrap = 0.0f, int align = 0, float thickness = 0.5f, float softness = 0.05f, DNvec4 outlineColor = {0.0f, 0.0f, 0.0f, 0.0f}, float outlineThickness = 1.0f, float outlineSoftness = 0.05f);

	void update(float dt, DNvec2 parentPos, DNvec2 parentSize);
	void render(float parentAlphaMult);
};

#endif