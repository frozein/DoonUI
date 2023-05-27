#include "textbox.hpp"
#include <math.h>

dnui::TextBox::TextBox(Coordinate x, Coordinate y, Dimension w, Dimension h, 
	std::string txt, DNUIfont* fnt, DNvec4 txtCol, float txtThick, DNvec4 txtOutlineCol,
	float txtOutlineThick, float txtPad, DNvec4 bckgroundCol, float bckgroundCornerRad, 
	DNvec4 bckgroundOutlineCol, float bckgroundOutlineThick, DNvec4 cursorCol) : dnui::Element::Element(x, y, w, h)
{
	m_text = txt;
	m_font = fnt;
	m_textColor = txtCol;
	m_textThickness = txtThick;
	m_textOutlineColor = txtOutlineCol;
	m_textOutlineThickness = txtOutlineThick;
	m_textPadding = txtPad;
	m_cursorTransition = Transition(250.0f, Transition::EXPONENTIAL);
	m_cursorAlpha = cursorCol.a;

	m_background = new Box(Coordinate(), Coordinate(), Dimension(), Dimension(), -1,
	                       bckgroundCol, bckgroundCornerRad, 0.0f, bckgroundOutlineCol,
						   bckgroundOutlineThick);
	
	m_cursor = new Box(Coordinate(), Coordinate(), Dimension(dnui::Dimension::PIXELS, CURSOR_SIZE), 
	                   Dimension(dnui::Dimension::SPACE, 2.0f * txtPad), -1, cursorCol);
	m_cursor->m_color.a = 0.0f;

	m_highlight = new Box(Coordinate(), Coordinate(), Dimension(), Dimension(dnui::Dimension::SPACE, 2.0f * txtPad),
	                      -1, cursorCol);
	m_highlight->m_color.a = 0.5f * cursorCol.a;
}

dnui::TextBox::~TextBox()
{
	delete m_background;
	delete m_cursor;
	delete m_highlight;
}

void dnui::TextBox::set_control_key_state(bool shiftHeld, bool ctrlHeld)
{
	s_shiftHeld = shiftHeld;
	s_ctrlHeld = ctrlHeld;
}

std::string dnui::TextBox::get_highlighted()
{
	if(m_highlightLen == 0)
		return m_text;
	else if(m_highlightLen < 0)
		return m_text.substr(m_cursorPos + m_highlightLen, -m_highlightLen);
	else
		return m_text.substr(m_cursorPos, m_highlightLen);
}

void dnui::TextBox::update(float dt, DNvec2 parentPos, DNvec2 parentSize)
{
	//for animating cursor:
	m_time += dt;

	//get text render scale:
	m_charPositons.resize(m_text.length());
	m_textSize = DNUI_line_render_size(m_text.c_str(), m_font, 1.0f, m_charPositons.data());

	DNvec2 expectedTextSize = DN_vec2_sub(m_renderSize, {2.0f * m_textPadding, 2.0f * m_textPadding});
	float expectedAspect = expectedTextSize.x / expectedTextSize.y;
	float textAspect = m_textSize.x / m_textSize.y;

	if(expectedAspect >= textAspect)
		m_renderScale = expectedTextSize.y / m_textSize.y;
	else
		m_renderScale = expectedTextSize.x / m_textSize.x;

	//handle user input:
	if(s_mousePressed && !s_mousePressedLast && is_hovered())
	{
		m_highlighting = true;
		m_hasFocus = true;
		m_orgCursorPos = get_cursor_pos(s_mousePos.x);
		m_time = 0.0f;
	}

	if(m_highlighting)
	{
		m_cursorPos = get_cursor_pos(s_mousePos.x);
		m_highlightLen = m_orgCursorPos - m_cursorPos;
	}

	//set cursor position:
	if(m_lastCursorPos != m_cursorPos)
	{
		m_cursorTransition.set_target_x(Coordinate(Coordinate::PIXELS, get_cursor_render_pos(m_cursorPos), Coordinate::CENTER_CENTER));
		m_cursor->set_transition(m_cursorTransition, 0.0f);
		m_time = 0.0f;

		m_lastCursorPos = m_cursorPos;
	}

	//highlight position:
	m_highlight->m_xPos.type = dnui::Coordinate::PIXELS;
	m_highlight->m_width.type = dnui::Dimension::PIXELS;

	float startPos = m_cursor->get_render_pos().x - m_renderPos.x;
	float endPos;
	if(m_highlightLen == 0)
		endPos = startPos;
	else
		endPos = get_cursor_render_pos(m_cursorPos + m_highlightLen);

	m_highlight->m_xPos.pixelPos = (startPos + endPos) * 0.5f;
	m_highlight->m_width.pixelSize = fabsf(endPos - startPos);

	if(m_hasFocus)
		m_cursor->m_color.a = m_cursorAlpha * (1.3f * atanf(sinf(m_time * 0.0075f + 1.5707f) / 0.4f) + 0.5f); //smoothed square wave
	else
		m_cursor->m_color.a = 0.0f;

	//update children:
	m_background->update(dt, m_renderPos, m_renderSize);
	m_cursor->update(dt, m_renderPos, m_renderSize);
	m_highlight->update(dt, m_renderPos, m_renderSize);

	Element::update(dt, parentPos, parentSize);

	//if cursor position has not yet been initially set, set it
	if(m_cursor->m_xPos.type == Coordinate::RELATIVE)
		m_cursor->m_xPos = Coordinate(Coordinate::PIXELS, get_cursor_render_pos(m_cursorPos), Coordinate::CENTER_CENTER);
}

void dnui::TextBox::render(float parentAlphaMult)
{
	m_background->render(m_alphaMult * parentAlphaMult);

	DNvec2 renderPos = {m_renderPos.x - m_renderSize.x * 0.5f + m_textSize.x * m_renderScale * 0.5f + m_textPadding, m_renderPos.y};
	DNvec4 renderCol = {m_textColor.x, m_textColor.y, m_textColor.z, m_textColor.w * m_alphaMult * parentAlphaMult};
	DNvec4 outlineRenderCol = {m_textOutlineColor.x, m_textOutlineColor.y, m_textOutlineColor.z, m_textOutlineColor.w * m_alphaMult * parentAlphaMult};
	DNUI_draw_string(m_text.c_str(), m_font, renderPos, m_renderScale, 0.0f, 0, renderCol, m_textThickness, 0.05f, outlineRenderCol, m_textOutlineThickness, 0.05f);

	m_cursor->render(m_alphaMult * parentAlphaMult);
	m_highlight->render(m_alphaMult * parentAlphaMult);
}

void dnui::TextBox::handle_event(Event event)
{
	switch(event.type)
	{
	case Event::MOUSE_RELEASE:
	{
		if(!m_highlighting && !is_hovered())
			m_hasFocus = false;

		m_highlighting = false;
		break;
	}
	case Event::ARROW_KEY_PRESS:
	{
		if(!m_hasFocus)
			break;

		int oldCursorPos = m_cursorPos;
		int oldHighlightLen = m_highlightLen;
		switch(event.arrowKey.dir)
		{
		case 0:
			if(m_cursorPos < m_text.length())
			{
				m_cursorPos++;
				m_highlightLen--;
			}
			break;
		case 1:
			if(m_cursorPos > 0)
			{
				m_cursorPos--;
				m_highlightLen++;
			}
			break;
		case 2:
			m_cursorPos = m_text.length();
			m_highlightLen = oldCursorPos - m_cursorPos;
			break;
		case 3:
			m_cursorPos = 0;
			m_highlightLen = oldCursorPos - m_cursorPos;
			break;
		}

		if(!s_shiftHeld)
		{
			if(oldCursorPos - m_cursorPos < 0 && oldHighlightLen > 0 ||
			   oldCursorPos - m_cursorPos > 0 && oldHighlightLen < 0)
				m_cursorPos = oldCursorPos + oldHighlightLen;
			else if(oldHighlightLen != 0)
				m_cursorPos = oldCursorPos;

			m_highlightLen = 0;
		}

		break;
	}
	case Event::DELETE_KEY_PRESS:
	{
		if(!m_hasFocus)
			break;

		int startPos;
		int endPos;
		if(m_highlightLen < 0)
		{
			startPos = m_cursorPos + m_highlightLen;
			endPos = m_cursorPos;
			m_cursorPos = startPos;
		}
		else if(m_highlightLen > 0)
		{
			startPos = m_cursorPos;
			endPos = m_cursorPos + m_highlightLen;
		}
		else
		{
			startPos = m_cursorPos;
			endPos = m_cursorPos;

			if(event.del.backspace)
			{
				if(m_cursorPos > 0)
				{
					startPos--;
					m_cursorPos--;
				}
			}
			else
			{
				if(m_cursorPos < m_text.length())
					endPos++;
			}
		}

		m_text = m_text.substr(0, startPos) + m_text.substr(endPos);
		m_highlightLen = 0;
		break;
	}
	case Event::CHARACTER:
	{
		if(!m_hasFocus)
			break;

		int startPos;
		int endPos;
		if(m_highlightLen < 0)
		{
			startPos = m_cursorPos + m_highlightLen;
			endPos = m_cursorPos;
			m_cursorPos = startPos;
		}
		else if(m_highlightLen > 0)
		{
			startPos = m_cursorPos;
			endPos = m_cursorPos + m_highlightLen;
		}
		else
		{
			startPos = m_cursorPos;
			endPos = m_cursorPos;
		}

		m_text = m_text.substr(0, startPos) + (char)event.character.character + m_text.substr(endPos);
		m_cursorPos++;
		m_highlightLen = 0;
		break;
	}
	default:
		break;
	}
}

float dnui::TextBox::get_cursor_render_pos(int cursorPos)
{
	if(m_text.length() == 0)
		return -m_renderSize.x * 0.5f + m_textPadding;

	float result;

	if(cursorPos == 0)
		result = m_charPositons[cursorPos].x * m_renderScale;
	else if(cursorPos == m_text.length())
	{
		result = m_charPositons[cursorPos - 1].y * m_renderScale;
	}
	else
	{
		result = (m_charPositons[cursorPos].x + m_charPositons[cursorPos - 1].y) * 0.5f * m_renderScale;
	}

	result += -m_renderSize.x * 0.5f + m_textPadding;
	return result;
}

int dnui::TextBox::get_cursor_pos(float cursorRenderPos)
{
	if(m_text.length() == 0)
		return 0;

	cursorRenderPos -= m_renderPos.x - m_renderSize.x * 0.5f + m_textPadding;
	cursorRenderPos = fmaxf(cursorRenderPos, 0.0f);

	for(int i = 0; i < m_charPositons.size(); i++)
	{
		float prevPos = i == 0 ? 0.0f : (m_charPositons[i - 1].x + m_charPositons[i - 1].y) * m_renderScale * 0.5f;
		float pos = (m_charPositons[i].x + m_charPositons[i].y) * m_renderScale * 0.5f;

		if(cursorRenderPos >= prevPos && cursorRenderPos < pos)
			return i;
	}

	return m_text.length();
}