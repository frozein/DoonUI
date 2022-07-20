#ifndef DNUI_ELEMENT_H
#define DNUI_ELEMENT_H

#include <vector>
#include <string>
#include "math/common.h"

//--------------------------------------------------------------------------------------------------------------------------------//
//DIMENSION/POSITION STRUCTS:

//represents a position for a DNUIelement
struct DNUIcoordinate
{
	//determines how the element will be positioned
	enum Type
	{
		RELATIVE, //the element will be positioned relative to its parent's size. For example, a relativePos of 0.5 would place the element in the center of its parent
		PIXELS	  //the element will be positioned relative to its parent's center, min, or max (depending on what center is set to), being offset by pixelPos pixels
	} type;

	//determines at what point the element will be centered
	enum Center
	{
		CENTER_MIN,	   //the element will be positioned centered at its minimum coordinate
		CENTER_CENTER, //the element will be positioned centered at its center
		CENTER_MAX	   //the element will be positioned centered at its maximum coordinate
	} center;

	union
	{
		float relativePos;
		float pixelPos;
	};

	DNUIcoordinate();
	DNUIcoordinate(DNUIcoordinate::Type type, float param, DNUIcoordinate::Center center);
};

struct DNUIdimension
{
	//determines how the element will be sized
	enum Type
	{
		RELATIVE, //the element will be sized relative to its parent's size. For example, a relativeSize of 0.5 would result in the element being half as large as it's parent
		PIXELS,	  //the element will have a fixed pixel size
		ASPECT,	  //the element's size will be relative to its size in the other dimension
		SPACE	  //the element will be sized such that it takes up emptyPixels fewer pixels that it's parent does
	} type;

	union
	{
		float relativeSize;
		float pixelSize;
		float aspectRatio;
		float emptyPixels;
	};

	DNUIdimension();
	DNUIdimension(DNUIdimension::Type type, float param);
};

//--------------------------------------------------------------------------------------------------------------------------------//
//EVENT STRUCT:

struct DNUIevent
{
	enum Type
	{
		NONE,
		MOUSE_RELEASE
	} type;

	//data here

	DNUIevent();
	DNUIevent(DNUIevent::Type type);
};

//--------------------------------------------------------------------------------------------------------------------------------//
//ELEMENT CLASS:

//a generic UI element, has no functionality on its own other than to update/render child elements
class DNUIelement
{
public:
	std::vector<DNUIelement*> children;

	DNUIcoordinate xPos;
	DNUIcoordinate yPos;

	DNUIdimension width;
	DNUIdimension height;

	DNUIelement();
	/* @param x the x position of the element
	 * @param y the y position of the element
	 * @param w the width of the element
	 * @param h the height of the element
	 */
	DNUIelement(DNUIcoordinate x, DNUIcoordinate y, DNUIdimension w, DNUIdimension h);
	//deletes all child elements, do NOT delete child elements yourself
	~DNUIelement();

	//calls update() on all child elements, can be overriden
	virtual void update(float dt, DNvec2 parentPos, DNvec2 parentSize);
	//calls render() on all child elements, can be overriden
	virtual void render();
	//calls handle_event() on all child elements, can be overriden
	virtual void handle_event(DNUIevent event);
};

//--------------------------------------------------------------------------------------------------------------------------------//
//DEFAULT ELEMENTS:

//a simple rectangle
class DNUIbox : public DNUIelement
{
protected:
	DNvec2 renderPos;  //the final position of the box's center, in pixels
	DNvec2 renderSize; //the final size of the box, in pixels

public:
	DNvec4 color;		//the box's color
	float cornerRadius; //the radius of the box's corners, in pixels
	int texture;		//the openGL texture handle to use when rendering, or -1 if no texture is desired

	DNUIbox();
	/* @param x the x position of the box
	 * @param y the y position of the box
	 * @param w the width of the box
	 * @param h the height of the box
	 * @param color the color of the box
	 * @param cornerRad the radius of the box's corners, in pixels
	 * @param tex the openGL texture handle to use when rendering, or -1 if no texture is desired
	 */
	DNUIbox(DNUIcoordinate x, DNUIcoordinate y, DNUIdimension w, DNUIdimension h, DNvec4 col, float cornerRad = 0.0f, int tex = -1);

	void update(float dt, DNvec2 parentPos, DNvec2 parentSize);
	void render();
};

//a button that calls a user-defined function when clicked
class DNUIbutton : public DNUIbox
{
private:
	inline static DNvec2 mousePos;  //the mouse's current screen position
	inline static bool mousePressed; //whether or not the mouse button is held down

public:
	void (*button_callback)(int); //the function that gets called when the button is clicked
	int callbackID;				  //the unique id that gets passed to the callback function, used to differentiate buttons that have the same callback func

	DNUIbutton();
	/* @param x the x position of the box
	 * @param y the y position of the box
	 * @param w the width of the box
	 * @param h the height of the box
	 * @param color the color of the box
	 * @param buttonCallback the function that gets called when this button is clicked
	 * @param callbackID the value that gets passed to the callback function, can be used to identify buttons that use the same callback func
	 * @param cornerRad the radius of the box's corners, in pixels
	 * @param tex the openGL texture handle to use when rendering, or -1 if no texture is desired
	 */
	DNUIbutton(DNUIcoordinate x, DNUIcoordinate y, DNUIdimension w, DNUIdimension h, DNvec4 color, void (*buttonCallback)(int), int callbackID = 0, float cornerRad = 0.0f, int tex = -1);

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
	DNvec2 renderPos;  //the final position of the text's center, in pixels
	float renderScale; //the final scale of text
	float renderW;	   //the final maximum line width of the text, in pixels

public:

	//text parameters:
	std::string text;		//the text to render
	int font;				//the handle to the font to render with
	DNvec4 color;			//the color of the text
	float scale;			//the scale of the text, or -1 for automatic scaling
	float lineWrap; 		//the maximum number of pixels the text can extend before wrapping, to 0.0 if no wrapping is desired
	int align; 				//how to align the text when wrapping: 0 = align left, 1 = align right, 2 = align center
	float thickness;	    //the thickness of the text
	float softness;			//the softness of the text's edges

	//outline parameters
	DNvec4 outlineColor; 	//the color of the text's outline
	float outlineThickness; //the thickness at which the text's outline begins
	float outlineSoftness;  //the softness of the outline's edges

	DNUItext();
	/* @param x the x position of the element
	 * @param y the y position of the element
	 * @param size the width of the element, the height is determined automatically to avoid stretching
	 * @param text the text to render
	 * @param font the handle to the font to render with
	 * @param color the color of the text
	 * @param scale the scale of the text, or -1 to allow for automatic scaling (otherwise the lineWrap will be changed and scale will remain constant)
	 * @param lineWrap the maximum number of pixels the text can extend before wrapping. Set to 0.0 if no wrapping is desired
	 * @param align how to align the text when wrapping: 0 = align left (all lines start at left side), 1 = align right (all lines end at right side), 2 = align center (all lines are individually centered)
	 * @param thickness the thickness of the text
	 * @param softness the softness of the text's edges
	 * @param outlineColor the color of the text's outline
	 * @param outlineThickness the thickness at which the text's outline begins, set to 1.0 if no outline is desired
	 * @param outlineSoftness the softness of the outline's edges
	 */
	DNUItext(DNUIcoordinate x, DNUIcoordinate y, DNUIdimension size, std::string text, int font, DNvec4 color = {1.0f, 1.0f, 1.0f, 0.0f}, float scale = 1.0f, float lineWrap = 0.0f, int align = 0, float thickness = 0.5f, float softness = 0.05f, DNvec4 outlineColor = {0.0f, 0.0f, 0.0f, 0.0f}, float outlineThickness = 1.0f, float outlineSoftness = 0.05f);

	void update(float dt, DNvec2 parentPos, DNvec2 parentSize);
	void render();
};

#endif