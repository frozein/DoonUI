#ifndef SLIDER_H
#define SLIDER_H

#include "box.hpp"
#include "button.hpp"
#include "../element.hpp"

namespace dnui
{

//a user-adjustable slider, does not display any text
class Slider : public Element
{
public:
	enum DataType
	{
		FLOAT,
		INT,
		DOUBLE
	};

	Box* m_base;      //the base of the slider, the "track" that the slider sits on
	Button* m_slider; //the adjustable part of the slider

	bool m_vertical = false; //whether the slider is vertical or horizontal

	DataType m_type;       //the type of data that this slider modifies
	void* m_val = nullptr; //the pointer to the actual data, varies in type
	float m_maxVal;        //the maximum value of the data
	float m_minVal;        //the minimum value of the data

	Slider() = default;
	Slider(Coordinate x, Coordinate y, Dimension w, Dimension h,
		DataType type, void* val, float minVal, float maxVal, bool vertical = false,
		DNvec4 baseCol = {1.0f, 1.0f, 1.0f, 1.0f}, DNvec4 baseOutlineCol = {0.0f, 0.0f, 0.0f, 1.0f},
		float baseOutlineThickness = 0.0f, DNvec4 sliderCol = {1.0f, 0.0f, 0.0f, 1.0f},
		DNvec4 sliderOutlineCol = {0.0f, 0.0f, 0.0f, 1.0f}, float sliderOutlineThickness = 0.0f, float sliderCornerRad = 10.0f,
		Transition sliderBase = Transition(), Transition sliderHover = Transition(), Transition sliderHold = Transition());
	Slider(const Slider& old);

	void update(float dt, DNvec2 parentPos, DNvec2 parentSize);

	/* Whether or not the slider is currently being dragged by the user
	 * @returns whether or not the slider is currently being dragged by the user
	 */
	bool is_moving();

protected:
	bool m_moving = false; //whether or now the user is currently moving the slider

	void check_moving();
	void update_val();
	void clamp_val();
	void set_slider_pos();
};

}; //namespace dnui

#endif