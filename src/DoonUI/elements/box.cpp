#include "box.hpp"

dnui::Box::Box(Coordinate x, Coordinate y, Dimension w, Dimension h, 
	int tex, DNvec4 col, float cornerRad, float agl, DNvec4 outlineCol, 
	float outlineThick) : dnui::Element::Element(x, y, w, h)
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
	DNvec4 renderOutlineCol = {m_outlineColor.x, m_outlineColor.y, m_outlineColor.z, m_outlineColor.w * m_alphaMult * parentAlphaMult};

	DNUI_draw_rect(m_texture, m_renderPos, m_renderSize, m_angle, renderCol, m_cornerRadius, renderOutlineCol, m_outlineThickness);
	dnui::Element::render(parentAlphaMult);
}