#include "colorselector.hpp"
#include <GLAD/glad.h>
#include <iostream>

unsigned int dnui::ColorSelector::s_colorTexture = -1;

dnui::ColorSelector::ColorSelector(Coordinate x, Coordinate y, Dimension w, Dimension h,
	DNvec3* selectedColor, DNvec4 backgroundCol, float backgroundCornerRad, DNvec4 outlineCol, 
	float outlineThickness, DNvec4 selectorCol, DNvec4 sliderBaseCol) : dnui::Element::Element(x, y, w, h)
{
	m_selectedColor = selectedColor;

	m_background = new Box(Coordinate(), Coordinate(), Dimension(), Dimension(), -1,
		backgroundCol, backgroundCornerRad, 0.0f, outlineCol, outlineThickness);
	
	m_selector = new Box(Coordinate(Coordinate::PIXELS, 20.0f, Coordinate::CENTER_MIN), Coordinate(), 
		Dimension(Dimension::ASPECT, 1.0f), Dimension(Dimension::SPACE, 40.0f), s_colorTexture,
		{1.0f, 1.0f, 1.0f, 1.0f}, 0.0f, 0.0f, outlineCol, outlineThickness * 0.5f);

	m_selectorButton = new Box(Coordinate(), Coordinate(), Dimension(Dimension::ASPECT, 1.0f), Dimension(Dimension::RELATIVE, 0.12f),
		-1, selectorCol, 10.0f, 0.0f, outlineCol, outlineThickness * 0.5f);

	m_valueSlider = new Slider(Coordinate(Coordinate::PIXELS, 20.0f, Coordinate::CENTER_MAX), Coordinate(),
		Dimension(Dimension::RELATIVE, 0.05f), Dimension(Dimension::SPACE, 40.0f), Slider::FLOAT, 
		&m_value, 0.0f, 1.0f, true, sliderBaseCol, outlineCol, outlineThickness * 0.5f, selectorCol,
		outlineCol, outlineThickness * 0.5f);

	m_selector->m_children.push_back(m_selectorButton);

	m_children.push_back(m_background);
	m_children.push_back(m_selector);
	m_children.push_back(m_valueSlider);
}

dnui::ColorSelector::ColorSelector(const ColorSelector& old) : dnui::Element::Element(old.m_xPos, old.m_yPos, old.m_width, old.m_height)
{
	m_selectedColor = old.m_selectedColor;

	m_alphaMult = old.m_alphaMult;
	m_activeTransition = old.m_activeTransition;
	m_transition = old.m_transition;

	m_background = new Box(*old.m_background);
	m_selector = new Box(*old.m_selector);
	m_selectorButton = new Box(*old.m_selectorButton);
	m_valueSlider = new Slider(*old.m_valueSlider);
	m_valueSlider->m_val = &m_value;
}

void dnui::ColorSelector::init_texture()
{
	const unsigned int WIDTH  = 512;
	const unsigned int HEIGHT = 512;
	DNvec4* tex = new DNvec4[WIDTH * HEIGHT];

	for(int i = 0; i < WIDTH; i++)
		for(int j = 0; j < HEIGHT; j++)
		{
			float s = (float)i / HEIGHT;
			float h = (float)j / WIDTH * 360.0f;

			DNvec3 col = hsv_to_rgb({h, s, 1.0f});
			tex[i * HEIGHT + j] = {col.r, col.g, col.b, 1.0f};
		}

	glGenTextures(1, &s_colorTexture);
	glBindTexture(GL_TEXTURE_2D, s_colorTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, tex);

	delete tex;
}

void dnui::ColorSelector::update(float dt, DNvec2 parentPos, DNvec2 parentSize)
{
	//CHECK IF COLOR SELECTOR IS BEING DRAGGED:
	if(!s_mousePressed)
		m_moving = false;

	DNvec2 selectorPos  = m_selector->get_render_pos();
	DNvec2 selectorSize = m_selector->get_render_size();
	bool hovered = s_mousePos.x >  selectorPos.x - selectorSize.x * 0.5f && s_mousePos.x <  selectorPos.x + selectorSize.x * 0.5f &&
	               s_mousePos.y > -selectorPos.y - selectorSize.y * 0.5f && s_mousePos.y < -selectorPos.y + selectorSize.y * 0.5f;
	
	if(hovered && s_mousePressed && !s_mousePressedLast)
		m_moving = true;

	//SET SELECTOR POSITION:
	if(DN_vec3_equals(*m_selectedColor, {0.0f, 0.0f, 0.0f}))
		hsvColor.z = 0.0f;
	else
		hsvColor = rgb_to_hsv(*m_selectedColor);

	m_selectorButton->m_xPos = Coordinate(Coordinate::RELATIVE, hsvColor.x / 360.0f, Coordinate::CENTER_CENTER);
	m_selectorButton->m_yPos = Coordinate(Coordinate::RELATIVE, hsvColor.y, Coordinate::CENTER_CENTER);
	m_value = hsvColor.z;

	//SET HUE AND SATURATION
	DNvec2 hs;
	if(m_moving && m_active) //from selector
	{
		selectorPos.y *= -1.0f;
		hs = DN_vec2_div(DN_vec2_sub(s_mousePos, selectorPos), selectorSize);
		hs.y *= -1.0f;
		hs = DN_vec2_add(hs, {0.5f, 0.5f});
		hs.x *= 360.0f;
	}
	else //from existing
		hs = {hsvColor.x, hsvColor.y};

	hs = DN_vec2_max(hs, {0.001f, 0.001f});
	hs = DN_vec2_min(hs, {359.9f, 0.999f});

	hsvColor.x = hs.x;
	hsvColor.y = hs.y;

	//SET ACTIVE:
	m_background->m_active     = m_active;
	m_selector->m_active       = m_active;
	m_selectorButton->m_active = m_active;
	m_valueSlider->m_active    = m_active;

	//SET SLIDER POSITION:
	Element::update(dt, parentPos, parentSize);

	//SET VALUE:
	float v;
	if(m_valueSlider->is_moving() && m_active)
		v = m_value;
	else
		v = hsvColor.z;

	hsvColor.z = v;

	//SET OUTPUT:
	m_selector->m_color = {m_value, m_value, m_value, 1.0f}; //NOTE: this is probably incorrect but looks fine
	*m_selectedColor = hsv_to_rgb(hsvColor);
}

DNvec3 dnui::ColorSelector::hsv_to_rgb(DNvec3 hsv)
{
	float c = hsv.z * hsv.y;
	float h2 = hsv.x / 60.0f;
	float x = c * (1.0f - fabsf(fmodf(h2, 2.0f) - 1.0f));

	DNvec3 rgb;
	if(h2 < 1.0f)
		rgb = {c, x, 0.0f};
	else if(h2 < 2.0f)
		rgb = {x, c, 0.0f};
	else if(h2 < 3.0f)
		rgb = {0.0f, c, x};
	else if(h2 < 4.0f)
		rgb = {0.0f, x, c};
	else if(h2 < 5.0f)
		rgb = {x, 0.0f, c};
	else if(h2 < 6.0f)
		rgb = {c, 0.0f, x};

	float m = hsv.z - c;
	rgb.r += m;
	rgb.g += m;
	rgb.b += m;

	return rgb;
}

DNvec3 dnui::ColorSelector::rgb_to_hsv(DNvec3 rgb)
{
	float cMax = fmaxf(fmaxf(rgb.r, rgb.g), rgb.b);
	float cMin = fminf(fminf(rgb.r, rgb.g), rgb.b);
	float delta = cMax - cMin;

	float h;
	if(delta == 0.0f)
		h = 0.0f;
	else if(cMax == rgb.r)
		h = (rgb.g - rgb.b) / delta + 6.0f;
	else if(cMax == rgb.g)
		h = (rgb.b - rgb.r) / delta + 2.0f;
	else if(cMax == rgb.b)
		h = (rgb.r - rgb.g) / delta + 4.0f;

	h = fmodf(60.0f * h, 360.0f);

	float s;
	if(cMax == 0.0f)
		s = 0.0f;
	else
		s = delta / cMax;
	
	float v = cMax;

	return {h, s, v};
}