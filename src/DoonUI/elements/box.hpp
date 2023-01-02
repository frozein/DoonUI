#ifndef BOX_H
#define BOX_H

#include "../element.hpp"

namespace dnui
{

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

}; //namespace dnui

#endif