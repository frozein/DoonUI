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

	Box* m_base;
	Button* m_slider;

	bool m_vertical = false;

	DataType m_type;
	void* m_val = nullptr;
	float m_maxVal;
	float m_minVal;

	Slider() = default;
	Slider(Coordinate x, Coordinate y, Dimension w, Dimension h,
		DataType type, void* val, float minVal, float maxVal, bool vertical = false,
		DNvec4 baseCol = {1.0f, 1.0f, 1.0f, 1.0f}, DNvec4 baseOutlineCol = {0.0f, 0.0f, 0.0f, 1.0f},
		float baseOutlineThickness = 0.0f, DNvec4 sliderCol = {1.0f, 0.0f, 0.0f, 1.0f},
		DNvec4 sliderOutlineCol = {0.0f, 0.0f, 0.0f, 1.0f}, float sliderOutlineThickness = 0.0f, float sliderCornerRad = 10.0f,
		Transition sliderBase = Transition(), Transition sliderHover = Transition(), Transition sliderHold = Transition());
	Slider(const Slider& old);

	void update(float dt, DNvec2 parentPos, DNvec2 parentSize);

protected:
	bool m_moving = false;

	void check_moving();
	void update_val();
	void clamp_val();
	void set_slider_pos();
};

}; //namespace dnui

#endif