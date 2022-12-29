#include "utility.hpp"
#include "element.hpp"

//--------------------------------------------------------------------------------------------------------------------------------//

dnui::Coordinate::Coordinate(dnui::Coordinate::Type t, float param, dnui::Coordinate::Center c)
{
	type = t;
	relativePos = param; //all parameters are single floats, so this will apply to all of them
	center = c;
};

float dnui::Coordinate::calc_render_pos(float parentPos, float parentSize, float size)
{
	float pos = 0.0f;

	//find position relative to parent:
	if(type == dnui::Coordinate::PIXELS)
		switch(center)
		{
		case dnui::Coordinate::CENTER_CENTER:
		{
			pos = parentPos + pixelPos;
			break;
		}
		case dnui::Coordinate::CENTER_MAX:
		{
			pos = parentPos + parentSize * 0.5f - pixelPos;
			break;
		}
		case dnui::Coordinate::CENTER_MIN:
		{
			pos = parentPos - parentSize * 0.5f + pixelPos;
		}
		}
	else if (type == dnui::Coordinate::RELATIVE)
		pos =  parentPos + parentSize * (relativePos - 0.5f);

	//account for centering:
	if(center == dnui::Coordinate::CENTER_MIN)
		pos += size * 0.5f;
	else if(center == dnui::Coordinate::CENTER_MAX)
		pos -= size * 0.5f;

	//return:
	return pos;
}

//--------------------------------------------------------------------------------------------------------------------------------//

dnui::Dimension::Dimension(dnui::Dimension::Type t, float param)
{
	type = t;
	relativeSize = param; //all parameters are single floats, so this will apply to all of them
}

float dnui::Dimension::calc_render_size(float parentSize)
{
	switch(type)
	{
	case dnui::Dimension::PIXELS:
		return pixelSize;
	case dnui::Dimension::RELATIVE:
		return parentSize * relativeSize;
	case dnui::Dimension::SPACE:
		return parentSize - emptyPixels;
	default:
		return parentSize;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------//

dnui::Event::Event(Event::Type typ)
{
	type = typ;
}

//--------------------------------------------------------------------------------------------------------------------------------//

dnui::Transition::Transition(float t, InterpolationType i)
{
	m_time = t;
	m_interpolateType = i;
}

void dnui::Transition::set_target_x(dnui::Coordinate x)
{
	int i = get_component(dnui::Transition::Component::COORDINATE, offsetof(Element, m_xPos));
	m_components[i].coordinate.target = x;
	m_components[i].coordinate.x = true;
}

void dnui::Transition::set_target_y(dnui::Coordinate y)
{
	int i = get_component(dnui::Transition::Component::COORDINATE, offsetof(Element, m_yPos));
	m_components[i].coordinate.target = y;
	m_components[i].coordinate.x = false;
}

void dnui::Transition::set_target_w(dnui::Dimension w)
{
	int i = get_component(dnui::Transition::Component::DIMENSION, offsetof(Element, m_width));
	m_components[i].dimension.target = w;
	m_components[i].dimension.w = true;
}

void dnui::Transition::set_target_h(dnui::Dimension h)
{
	int i = get_component(dnui::Transition::Component::DIMENSION, offsetof(Element, m_height));
	m_components[i].dimension.target = h;
	m_components[i].dimension.w = false;
}

void dnui::Transition::set_target_alphamult(float a)
{
	set_target_float(a, offsetof(dnui::Element, m_alphaMult));
}

void dnui::Transition::set_target_float(float target, size_t offset)
{
	int i = get_component(dnui::Transition::Component::FLOAT, offset);
	m_components[i].floating.target = target;
}

void dnui::Transition::set_target_vec2(DNvec2 target, size_t offset)
{
	int i = get_component(dnui::Transition::Component::VEC2, offset);
	m_components[i].vector2.target = target;
}

void dnui::Transition::set_target_vec3(DNvec3 target, size_t offset)
{
	int i = get_component(dnui::Transition::Component::VEC3, offset);
	m_components[i].vector3.target = target;
}

void dnui::Transition::set_target_vec4(DNvec4 target, size_t offset)
{
	int i = get_component(dnui::Transition::Component::VEC4, offset);
	m_components[i].vector4.target = target;
}

void dnui::Transition::init(dnui::Element* element, float d)
{
	m_delay = d;

	for(int i = 0; i < m_components.size(); i++)
	{
		Component comp = m_components[i];
		char* ptr = (char*)element + comp.m_offset;

		switch(comp.m_type)
		{
		case dnui::Transition::Component::COORDINATE:
			m_components[i].coordinate.original = *(dnui::Coordinate*)ptr;
			break;
		case dnui::Transition::Component::DIMENSION:
			m_components[i].dimension.original = *(dnui::Dimension*)ptr;
			break;
		case dnui::Transition::Component::FLOAT:
			m_components[i].floating.original = *(float*)ptr;
			break;
		case dnui::Transition::Component::VEC2:
			m_components[i].vector2.original = *(DNvec2*)ptr;
			break;
		case dnui::Transition::Component::VEC3:
			m_components[i].vector3.original = *(DNvec3*)ptr;
			break;
		case dnui::Transition::Component::VEC4:
			m_components[i].vector4.original = *(DNvec4*)ptr;
			break;
		default:
			break;
		}
	}
}

bool dnui::Transition::update(float dt, dnui::Element* element, DNvec2 parentSize, DNvec2 size)
{
	//wait for delay:
	//---------------------------------
	if(m_delay > 0.0f)
	{
		m_delay -= dt;
		return true;
	}

	//update alpha:
	//---------------------------------
	m_alpha += dt / m_time;
	if(m_alpha > 1.0f)
		m_alpha = 1.0f;

	bool finished = m_alpha == 1.0f;

	//calculate corrected alpha (allows for different interpolation types):
	//---------------------------------
	float correctedAlpha;
	switch(m_interpolateType)
	{
	case dnui::Transition::QUADRATIC:
		correctedAlpha = 1.0f - (1.0f - m_alpha) * (1.0f - m_alpha);
		break;
	case dnui::Transition::CUBIC:
		correctedAlpha = 1.0f - (1.0f - m_alpha) * (1.0f - m_alpha) * (1.0f - m_alpha);
		break;
	case dnui::Transition::EXPONENTIAL:
		correctedAlpha = m_alpha == 1.0f ? 1.0f : 1.0f - powf(2.0f, -10.0f * m_alpha);
		break;
	case dnui::Transition::LINEAR:
	default:
		correctedAlpha = m_alpha;
		break;
	}

	//update components:
	//---------------------------------
	for(int i = 0; i < m_components.size(); i++)
	{
		Component comp = m_components[i];
		char* ptr = (char*)element + comp.m_offset;

		switch(comp.m_type)
		{
		case dnui::Transition::Component::COORDINATE:
		{
			float pS = comp.coordinate.x ? parentSize.x : parentSize.y; //parent size
			float s  = comp.coordinate.x ? size.x : size.y; //size

			float org = comp.coordinate.original.calc_render_pos(0.0f, pS, s);
			float tar = comp.coordinate.target.calc_render_pos(0.0f, pS, s);

			*(dnui::Coordinate*)ptr = finished ? comp.coordinate.target : dnui::Coordinate(dnui::Coordinate::PIXELS, lerp(org, tar, correctedAlpha), dnui::Coordinate::CENTER_CENTER);
			break;
		}
		case dnui::Transition::Component::DIMENSION:
		{
			float pS = comp.dimension.w ? parentSize.x : parentSize.y; //parent size
			float oS = comp.dimension.w ? size.y : size.x; //other size

			float org = comp.dimension.original.type == dnui::Dimension::ASPECT ? oS * comp.dimension.original.aspectRatio : comp.dimension.original.calc_render_size(pS);
			float tar = comp.dimension.target.type   == dnui::Dimension::ASPECT ? oS * comp.dimension.target.aspectRatio   : comp.dimension.target.calc_render_size(pS);

			*(dnui::Dimension*)ptr = finished ? comp.dimension.target : dnui::Dimension(dnui::Dimension::PIXELS, lerp(org, tar, correctedAlpha));
			break;
		}
		case dnui::Transition::Component::FLOAT:
			*(float*)ptr = finished ? comp.floating.target : lerp(comp.floating.original, comp.floating.target, correctedAlpha);
			break;
		case dnui::Transition::Component::VEC2:
			*(DNvec2*)ptr = finished ? comp.vector2.target : lerp(comp.vector2.original, comp.vector2.target, correctedAlpha);
			break;
		case dnui::Transition::Component::VEC3:
			*(DNvec3*)ptr = finished ? comp.vector3.target : lerp(comp.vector3.original, comp.vector3.target, correctedAlpha);
			break;
		case dnui::Transition::Component::VEC4:
			*(DNvec4*)ptr = finished ? comp.vector4.target : lerp(comp.vector4.original, comp.vector4.target, correctedAlpha);
			break;
		default:
			break;
		}
	}

	//return whether the transition has finished:
	//---------------------------------
	return !finished;
}

int dnui::Transition::get_component(Component::DataType type, size_t offset)
{
	int index = -1;
	for(int i = 0; i < m_components.size(); i++)
		if(m_components[i].m_offset == offset && m_components[i].m_type == type)
		{
			index = i;
			break;
		}

	if(index < 0)
	{
		index = m_components.size();
		m_components.push_back({});
	}

	m_components[index].m_type = type;
	m_components[index].m_offset = offset;
	return index;
}

float dnui::Transition::lerp(float a, float b, float alpha)
{
	return a + alpha * (b - a);
}

DNvec2 dnui::Transition::lerp(DNvec2 a, DNvec2 b, float alpha)
{
	return {lerp(a.x, b.x, alpha), lerp(a.y, b.y, alpha)};
}

DNvec3 dnui::Transition::lerp(DNvec3 a, DNvec3 b, float alpha)
{
	return {lerp(a.x, b.x, alpha), lerp(a.y, b.y, alpha), lerp(a.z, b.z, alpha)};
}

DNvec4 dnui::Transition::lerp(DNvec4 a, DNvec4 b, float alpha)
{
	return {lerp(a.x, b.x, alpha), lerp(a.y, b.y, alpha), lerp(a.z, b.z, alpha), lerp(a.w, b.w, alpha)};
}