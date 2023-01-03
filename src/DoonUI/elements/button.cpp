#include "button.hpp"

dnui::Button::Button(Coordinate x, Coordinate y, Dimension w, Dimension h, 
	void (*buttonCallback)(int), int id, int tex, DNvec4 col, float cornerRad, 
	float angle, DNvec4 outlineCol, float outlineThick, Transition base, 
	Transition hover, Transition hold) : dnui::Box::Box(x, y, w, h, tex, col, cornerRad, angle, outlineCol, outlineThick)
{
	m_button_callback = buttonCallback;
	m_callbackID = id;
	m_baseTransition = base;
	m_hoverTransition = hover;
	m_holdTransition = hold;
}

void dnui::Button::update_transitions()
{
	m_curState = -1;
}

void dnui::Button::update(float dt, DNvec2 parentPos, DNvec2 parentSize)
{
	if(s_mousePos.x >  m_renderPos.x - m_renderSize.x * 0.5f && s_mousePos.x <  m_renderPos.x + m_renderSize.x * 0.5f &&
	   s_mousePos.y > -m_renderPos.y - m_renderSize.y * 0.5f && s_mousePos.y < -m_renderPos.y + m_renderSize.y * 0.5f)
	{
		if(s_mousePressed)
		{
			if(m_curState != 2)
			{
				m_curState = 2;
				set_transition(m_holdTransition, 0.0f);
			}
		}
		else if(m_curState != 1)
		{
			m_curState = 1;
			set_transition(m_hoverTransition, 0.0f);
		}
	}
	else if(m_curState != 0)
	{
		m_curState = 0;
		set_transition(m_baseTransition, 0.0f);
	}

	dnui::Box::update(dt, parentPos, parentSize);
}

void dnui::Button::handle_event(Event event)
{
	if(event.type == Event::MOUSE_RELEASE &&
	   s_mousePos.x >  m_renderPos.x - m_renderSize.x * 0.5f && s_mousePos.x <  m_renderPos.x + m_renderSize.x * 0.5f &&
	   s_mousePos.y > -m_renderPos.y - m_renderSize.y * 0.5f && s_mousePos.y < -m_renderPos.y + m_renderSize.y * 0.5f)
	{
		if(m_button_callback != nullptr && m_active)
			m_button_callback(m_callbackID);
	}

	dnui::Element::handle_event(event);
}