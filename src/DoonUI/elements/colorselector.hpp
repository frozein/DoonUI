#ifndef COLORSELECTOR_H
#define COLORSELECTOR_H

#include "../element.hpp"
#include "box.hpp"
#include "slider.hpp"

namespace dnui
{

//an rgb color selector, does all calculations in hsv space and outputs an rgb color
class ColorSelector : public Element
{
public:
	Box* m_background;     //the background of the slider
	Box* m_selector;       //the box containing the color selecting texture
	Box* m_selectorButton; //the button sitting on top of the currently selected color
	Slider* m_valueSlider; //the slider for controling the value of the color (in hsv space)

	DNvec3* m_selectedColor = nullptr; //the pointer to the color that is selected, modified by this class

	ColorSelector() = default;
	ColorSelector(Coordinate x, Coordinate y, Dimension w, Dimension h,
		DNvec3* selectedColor, DNvec4 backgroundCol = {1.0f, 1.0f, 1.0f, 1.0f}, float backgroundCornerRad = 0.0f,
		DNvec4 outlineCol = {0.0f, 0.0f, 0.0f, 1.0f}, float outlineThickness = 0.0f,
		DNvec4 selectorCol = {1.0f, 1.0f, 1.0f, 1.0f}, DNvec4 sliderBaseCol = {0.0f, 0.0f, 0.0f, 1.0f});
	ColorSelector(const ColorSelector& old);

	/* Initlializes the color selecting texture, must be called before any objects are instantiated
	 */
	static void init_texture();

	void update(float dt, DNvec2 parentPos, DNvec2 parentSize);

protected:
	bool m_moving = false; //whether or not the user is currently moving the selector

	float m_value = 1.0f;                 //the value of the hsv color, modified by the slider
	DNvec3 hsvColor = {0.5f, 0.5f, 1.0f}; //the currently selected color in hsv space

	static DNvec3 hsv_to_rgb(DNvec3 hsv);
	static DNvec3 rgb_to_hsv(DNvec3 hsv);

private:
	static unsigned int s_colorTexture; //a handle to the color selecting texture, created in init_texture
};

}; //namespace dnui

#endif