#include "element.hpp"
#include "render.h"

//--------------------------------------------------------------------------------------------------------------------------------//

dnui::Element::Element(Coordinate x, Coordinate y, Dimension w, Dimension h)
{
	m_xPos = x;
	m_yPos = y;
	m_width = w;
	m_height = h;
}

dnui::Element::~Element()
{
	for(int i = 0; i < m_children.size(); i++)
		delete m_children[i];
}

void dnui::Element::update(float dt, DNvec2 parentPos, DNvec2 parentSize)
{
	calc_render_size(parentSize);
	calc_render_pos(parentPos, parentSize);

	if(m_activeTransition)
		m_activeTransition = m_transition.update(dt, this, parentSize, m_renderSize);

	for(int i = 0; i < m_children.size(); i++)
		m_children[i]->update(dt, m_renderPos, m_renderSize);
}

void dnui::Element::render(float parentAlphaMult)
{
	for(int i = 0; i < m_children.size(); i++)
		m_children[i]->render(m_alphaMult * parentAlphaMult);
}

void dnui::Element::handle_event(Event event)
{
	for(int i = 0; i < m_children.size(); i++)
		m_children[i]->handle_event(event);
}

void dnui::Element::set_transition(Transition trans, float delay)
{
	m_activeTransition = true;
	m_transition = trans;
	m_transition.init(this, delay);
}

void dnui::Element::calc_render_size(DNvec2 parentSize)
{
	if(m_width.type == Dimension::ASPECT)
	{
		float h = m_height.calc_render_size(parentSize.y);
		m_renderSize = {h * m_width.aspectRatio, h};
	}
	else if(m_height.type == Dimension::ASPECT)
	{
		float w = m_width.calc_render_size(parentSize.x);
		m_renderSize = {w, w * m_height.aspectRatio};
	}
	else
		m_renderSize = {m_width.calc_render_size(parentSize.x), m_height.calc_render_size(parentSize.y)};
}

void dnui::Element::calc_render_pos(DNvec2 parentPos, DNvec2 parentSize)
{
	m_renderPos = {m_xPos.calc_render_pos(parentPos.x, parentSize.x, m_renderSize.x), m_yPos.calc_render_pos(parentPos.y, parentSize.y, m_renderSize.y)};
}

//--------------------------------------------------------------------------------------------------------------------------------//

dnui::Box::Box(Coordinate x, Coordinate y, Dimension w, Dimension h, int tex, DNvec4 col, float cornerRad, float agl, DNvec4 outlineCol, float outlineThick) : dnui::Element::Element(x, y, w, h)
{
	m_texture = tex;
	m_color = col;
	m_cornerRadius = cornerRad;
	m_angle = agl;
	m_outlineColor = outlineCol;
	m_outlineThickness = outlineThick;
}

void dnui::Box::render(float parentAlphaMult)
{
	DNvec4 renderCol = {m_color.x, m_color.y, m_color.z, m_color.w * m_alphaMult * parentAlphaMult};
	DNUI_draw_rect(m_texture, m_renderPos, m_renderSize, m_angle, renderCol, m_cornerRadius, m_outlineColor, m_outlineThickness);
	dnui::Element::render(parentAlphaMult);
}

//--------------------------------------------------------------------------------------------------------------------------------//

dnui::Button::Button(Coordinate x, Coordinate y, Dimension w, Dimension h, void (*buttonCallback)(int), int id, int tex, DNvec4 col, float cornerRad, float angle, DNvec4 outlineCol, float outlineThick, Transition base, Transition hover, Transition hold) : dnui::Box::Box(x, y, w, h, tex, col, cornerRad, angle, outlineCol, outlineThick)
{
	m_button_callback = buttonCallback;
	m_callbackID = id;
	m_baseTransition = base;
	m_hoverTransition = hover;
	m_holdTransition = hold;
}

void dnui::Button::set_mouse_state(DNvec2 pos, bool pressed)
{
	s_mousePos = pos;
	s_mousePressed = pressed;
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
		if(m_button_callback != nullptr)
			m_button_callback(m_callbackID);
	}

	dnui::Element::handle_event(event);
}

//--------------------------------------------------------------------------------------------------------------------------------//

dnui::Text::Text(Coordinate x, Coordinate y, Dimension size, std::string txt, DNUIfont* fnt, DNvec4 col, float scl, float lnW, int algn, float thick, float soft, DNvec4 outlineCol, float outlineThick, float outlineSoft)
{
	m_xPos = x;
	m_yPos = y;
	m_width = size;
	m_height = Dimension(Dimension::RELATIVE, 1.0f);

	m_text = txt;
	m_color = col;
	m_font = fnt;
	m_scale = scl;
	m_lineWrap = lnW;
	m_align = algn;
	m_thickness = thick;
	m_softness = soft;
	m_outlineColor = outlineCol;
	m_outlineThickness = outlineThick;
	m_outlineSoftness = outlineSoft;
}

void dnui::Text::update(float dt, DNvec2 parentPos, DNvec2 parentSize)
{
	m_height.type = Dimension::PIXELS;
	if(m_lineWrap <= 0.0f)
	{
		if(m_scale <= 0.0f)
			m_renderScale = m_renderSize.x / DNUI_string_render_size(m_text.c_str(), m_font, 1.0f, 0.0f).x;
		else
		{
			m_renderScale = m_scale;

			m_width.type = Dimension::PIXELS;
			m_width.pixelSize = DNUI_string_render_size(m_text.c_str(), m_font, m_renderScale, 0.0f).x;
		}

		m_renderW = 0.0f;
		m_height.pixelSize = m_font->atlasH * m_renderScale;
	}
	else
	{
		if(m_scale <= 0.0f)
			m_renderScale = m_renderSize.x / m_lineWrap;
		else
			m_renderScale = m_scale;

		m_renderW = m_renderSize.x;
		m_height.pixelSize = DNUI_string_render_size(m_text.c_str(), m_font, m_renderScale, m_renderW).y;
	}

	dnui::Element::update(dt, parentPos, parentSize);
}

void dnui::Text::render(float parentAlphaMult)
{
	DNvec4 renderCol = {m_color.x, m_color.y, m_color.z, m_color.w * m_alphaMult * parentAlphaMult};
	DNvec4 outlineRenderCol = {m_outlineColor.x, m_outlineColor.y, m_outlineColor.z, m_outlineColor.w * m_alphaMult * parentAlphaMult};
	if(m_font != nullptr)
		DNUI_draw_string(m_text.c_str(), m_font, m_renderPos, m_renderScale, m_renderW, m_align, renderCol, m_thickness, m_softness, outlineRenderCol, m_outlineThickness, m_outlineSoftness);

	dnui::Element::render(parentAlphaMult);
}