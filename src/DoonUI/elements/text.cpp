#include "text.hpp"

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