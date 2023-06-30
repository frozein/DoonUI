#include "checkbox.hpp"

dnui::Checkbox::Checkbox(Coordinate x, Coordinate y, Dimension w, Dimension h, 
		bool* val, void (*buttonCallback)(int, bool, void*), int id, void* userData, int checkedTex, 
		int uncheckedTex, DNvec4 col, float cornerRad, float angle, DNvec4 outlineCol, 
		float outlineThick, Transition checkTransition, Transition uncheckTransition) : dnui::Box::Box(x, y, w, h, uncheckedTex, col, cornerRad, angle, outlineCol, outlineThick)
{
	m_val = val;
	m_button_callback = buttonCallback;
	m_callbackID = id;
	m_userData = userData;
	m_checkedTex = checkedTex;
	m_uncheckedTex = uncheckedTex;
	m_checkTransition = checkTransition;
	m_uncheckTransition = uncheckTransition;
}

void dnui::Checkbox::update(float dt, DNvec2 parentPos, DNvec2 parentSize)
{
	if(m_val)
		m_texture = *m_val ? m_checkedTex : m_uncheckedTex;

	dnui::Box::update(dt, parentPos, parentSize);
}

void dnui::Checkbox::handle_event(Event event)
{
	if(m_active && event.type == Event::MOUSE_RELEASE && is_hovered() && m_val)
	{
		*m_val = !*m_val;
		set_transition(*m_val ? m_checkTransition : m_uncheckTransition, 0.0f);

		if(m_button_callback != nullptr)
			m_button_callback(m_callbackID, *m_val, m_userData);
	}

	dnui::Element::handle_event(event);
}