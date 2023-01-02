#ifndef TEXT_H
#define TEXT_H

#include "../element.hpp"
#include <string>

namespace dnui
{

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