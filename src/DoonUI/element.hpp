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
};

//--------------------------------------------------------------------------------------------------------------------------------//
//DEFAULT ELEMENTS:

class DNUIbox : public DNUIelement
{
private:
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