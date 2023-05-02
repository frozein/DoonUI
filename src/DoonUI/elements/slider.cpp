#include "slider.hpp"
#include <math.h>

dnui::Slider::Slider(Coordinate x, Coordinate y, Dimension w, Dimension h, 
	DataType type, void* val, float minVal, float maxVal, bool vertical, DNvec4 baseCol, 
	DNvec4 baseOutlineCol, float baseOutlineThickness, DNvec4 sliderCol, 
	DNvec4 sliderOutlineCol, float sliderOutlineThickness, float sliderCornerRad, 
	Transition sliderBase, Transition sliderHover, Transition sliderHold) : dnui::Element::Element(x, y, w, h)
{
	m_type = type;
	m_val = val;
	m_minVal = minVal;
	m_maxVal = maxVal;
	m_vertical = vertical;

	Dimension baseW, baseH;
	Dimension sliderW, sliderH;
	if(m_vertical)
	{
		baseW = Dimension(Dimension::RELATIVE, 0.5f);
		baseH = Dimension();

		sliderW = Dimension();
		sliderH = Dimension(Dimension::ASPECT, 1.0f);
	}
	else
	{
		baseW = Dimension();
		baseH = Dimension(Dimension::RELATIVE, 0.5f);

		sliderW = Dimension(Dimension::ASPECT, 1.0f);
		sliderH = Dimension();
	}

	m_base = new Box(Coordinate(), Coordinate(), baseW, baseH,
		-1, baseCol, 0.0f, 0.0f, baseOutlineCol, baseOutlineThickness);
	
	m_slider = new Button(Coordinate(), Coordinate(), sliderW, sliderH,
		nullptr, 0, nullptr, -1, sliderCol, sliderCornerRad, 0.0f, sliderOutlineCol, 
		sliderOutlineThickness, sliderBase, sliderHover, sliderHold);

	m_children.push_back(m_base);
	m_children.push_back(m_slider);
}

dnui::Slider::Slider(const Slider& old) : dnui::Element::Element(old.m_xPos, old.m_yPos, old.m_width, old.m_height)
{
	m_type = old.m_type;
	m_val = old.m_val;
	m_minVal = old.m_minVal;
	m_maxVal = old.m_maxVal;
	m_vertical = old.m_vertical;
	m_active = old.m_active;

	m_alphaMult = old.m_alphaMult;
	m_activeTransition = old.m_activeTransition;
	m_transition = old.m_transition;

	m_base = new Box(*old.m_base);
	m_slider = new Button(*old.m_slider);

	m_children.push_back(m_base);
	m_children.push_back(m_slider);
}

void dnui::Slider::update(float dt, DNvec2 parentPos, DNvec2 parentSize)
{
	check_moving();
	if(m_moving && m_active)
		update_val();
	clamp_val();
	set_slider_pos();

	m_base->m_active = m_active;
	m_slider->m_active = m_active;

	dnui::Element::update(dt, parentPos, parentSize);
}

void dnui::Slider::check_moving()
{
	if(!s_mousePressed)
		m_moving = false;

	bool hovered = s_mousePos.x >  m_renderPos.x - m_renderSize.x * 0.5f && s_mousePos.x <  m_renderPos.x + m_renderSize.x * 0.5f &&
	               s_mousePos.y > -m_renderPos.y - m_renderSize.y * 0.5f && s_mousePos.y < -m_renderPos.y + m_renderSize.y * 0.5f;

	if(hovered && s_mousePressed && !s_mousePressedLast)
		m_moving = true;
}

void dnui::Slider::clamp_val()
{
	float floatVal;
	switch(m_type)
	{
	case FLOAT:
		floatVal = *(float*)m_val;
		break;
	case INT:
		floatVal = (float)*(int*)m_val;
		break;
	case DOUBLE:
		floatVal = (float)*(double*)m_val;
		break;
	}

	floatVal = fmin(fmax(floatVal, m_minVal), m_maxVal);

	switch(m_type)
	{
	case FLOAT:
		*(float*)m_val = floatVal;
		break;
	case INT:
		*(int*)m_val = (int)(floatVal);	
		break;
	case DOUBLE:
		*(double*)m_val = (double)(floatVal);
		break;
	}
}

void dnui::Slider::update_val()
{
	float minPos, maxPos;
	float sliderPos;
	if(m_vertical)
	{
		minPos = m_renderPos.y - m_renderSize.y * 0.5f;
		maxPos = m_renderPos.y + m_renderSize.y * 0.5f;
		sliderPos = -s_mousePos.y;
	}
	else
	{
		minPos = m_renderPos.x - m_renderSize.x * 0.5f;
		maxPos = m_renderPos.x + m_renderSize.x * 0.5f;
		sliderPos = s_mousePos.x;
	}

	sliderPos = (sliderPos - minPos) / (maxPos - minPos);

	float newVal = m_minVal + sliderPos * (m_maxVal - m_minVal);
	switch(m_type)
	{
	case FLOAT:
		*(float*)m_val = newVal;
		break;
	case INT:
		*(int*)m_val = (int)(newVal);	
		break;
	case DOUBLE:
		*(double*)m_val = (double)(newVal);
		break;
	}
}

void dnui::Slider::set_slider_pos()
{
	float newPos;
	switch(m_type)
	{
	case FLOAT:
		newPos = *(float*)m_val;
		break;
	case INT:
		newPos = (float)*(int*)m_val;
		break;
	case DOUBLE:
		newPos = (float)*(double*)m_val;
		break;
	}

	newPos = (newPos - m_minVal) / (m_maxVal - m_minVal);
	Coordinate newCoord = Coordinate(Coordinate::RELATIVE, newPos, Coordinate::CENTER_CENTER);

	if(m_vertical)
		m_slider->m_yPos = newCoord;
	else
		m_slider->m_xPos = newCoord;
}

bool dnui::Slider::is_moving()
{
	return m_moving;
}