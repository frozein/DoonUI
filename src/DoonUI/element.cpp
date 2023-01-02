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