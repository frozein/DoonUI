#include "utility.hpp"
#include "element.hpp"

//--------------------------------------------------------------------------------------------------------------------------------//

DNUIcoordinate::DNUIcoordinate(DNUIcoordinate::Type t, float param, DNUIcoordinate::Center c)
{
	type = t;
	relativePos = param; //all parameters are single floats, so this will apply to all of them
	center = c;
};

float DNUIcoordinate::calc_render_pos(float parentPos, float parentSize, float size)
{
	float pos = 0.0f;

	//find position relative to parent:
	if(type == DNUIcoordinate::PIXELS)
		switch(center)
		{
		case DNUIcoordinate::CENTER_CENTER:
		{
			pos = parentPos + pixelPos;
			break;
		}
		case DNUIcoordinate::CENTER_MAX:
		{
			pos = parentPos + parentSize * 0.5f - pixelPos;
			break;
		}
		case DNUIcoordinate::CENTER_MIN:
		{
			pos = parentPos - parentSize * 0.5f + pixelPos;
		}
		}
	else if (type == DNUIcoordinate::RELATIVE)
		pos =  parentPos + parentSize * (relativePos - 0.5f);

	//account for centering:
	if(center == DNUIcoordinate::CENTER_MIN)
		pos += size * 0.5f;
	else if(center == DNUIcoordinate::CENTER_MAX)
		pos -= size * 0.5f;

	//return:
	return pos;
}

//--------------------------------------------------------------------------------------------------------------------------------//

DNUIdimension::DNUIdimension(DNUIdimension::Type t, float param)
{
	type = t;
	relativeSize = param; //all parameters are single floats, so this will apply to all of them
}

float DNUIdimension::calc_render_size(float parentSize)
{
	switch(type)
	{
	case DNUIdimension::PIXELS:
		return pixelSize;
	case DNUIdimension::RELATIVE:
		return parentSize * relativeSize;
	case DNUIdimension::SPACE:
		return parentSize - emptyPixels;
	default:
		return parentSize;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------//

DNUIevent::DNUIevent(DNUIevent::Type typ)
{
	type = typ;
}

//--------------------------------------------------------------------------------------------------------------------------------//

DNUItransition::DNUItransition(float t, InterpolationType i)
{
	time = t;
	interpolateType = i;
}

int DNUItransition::get_component(DNUItransition::Component::DataType type, size_t offset)
{
	int index = -1;
	for(int i = 0; i < components.size(); i++)
		if(components[i].offset == offset && components[i].type == type)
		{
			index = i;
			break;
		}

	if(index < 0)
	{
		index = components.size();
		components.push_back({});
	}

	components[index].type = type;
	components[index].offset = offset;
	return index;
}

void DNUItransition::set_target_x(DNUIcoordinate x)
{
	int i = get_component(DNUItransition::Component::COORDINATE, offsetof(DNUIelement, xPos));
	components[i].coordinate.target = x;
	components[i].coordinate.x = true;
}

void DNUItransition::set_target_y(DNUIcoordinate y)
{
	int i = get_component(DNUItransition::Component::COORDINATE, offsetof(DNUIelement, yPos));
	components[i].coordinate.target = y;
	components[i].coordinate.x = false;
}

void DNUItransition::set_target_w(DNUIdimension w)
{
	int i = get_component(DNUItransition::Component::DIMENSION, offsetof(DNUIelement, width));
	components[i].dimension.target = w;
	components[i].dimension.w = true;
}

void DNUItransition::set_target_h(DNUIdimension h)
{
	int i = get_component(DNUItransition::Component::DIMENSION, offsetof(DNUIelement, height));
	components[i].dimension.target = h;
	components[i].dimension.w = false;
}

void DNUItransition::set_target_alphamult(float a)
{
	set_target_float(a, offsetof(DNUIelement, alphaMult));
}

void DNUItransition::set_target_float(float target, size_t offset)
{
	int i = get_component(DNUItransition::Component::FLOAT, offset);
	components[i].floating.target = target;
}

void DNUItransition::set_target_vec2(DNvec2 target, size_t offset)
{
	int i = get_component(DNUItransition::Component::VEC2, offset);
	components[i].vector2.target = target;
}

void DNUItransition::set_target_vec3(DNvec3 target, size_t offset)
{
	int i = get_component(DNUItransition::Component::VEC3, offset);
	components[i].vector3.target = target;
}

void DNUItransition::set_target_vec4(DNvec4 target, size_t offset)
{
	int i = get_component(DNUItransition::Component::VEC4, offset);
	components[i].vector4.target = target;
}

void DNUItransition::init(DNUIelement* element, float d)
{
	delay = d;

	for(int i = 0; i < components.size(); i++)
	{
		Component comp = components[i];
		char* ptr = (char*)element + comp.offset;

		switch(comp.type)
		{
		case DNUItransition::Component::COORDINATE:
			components[i].coordinate.original = *(DNUIcoordinate*)ptr;
			break;
		case DNUItransition::Component::DIMENSION:
			components[i].dimension.original = *(DNUIdimension*)ptr;
			break;
		case DNUItransition::Component::FLOAT:
			components[i].floating.original = *(float*)ptr;
			break;
		case DNUItransition::Component::VEC2:
			components[i].vector2.original = *(DNvec2*)ptr;
			break;
		case DNUItransition::Component::VEC3:
			components[i].vector3.original = *(DNvec3*)ptr;
			break;
		case DNUItransition::Component::VEC4:
			components[i].vector4.original = *(DNvec4*)ptr;
			break;
		default:
			break;
		}
	}
}

static inline float _DNUI_lerp(float a, float b, float alpha)
{
	return a + alpha * (b - a);
}

static inline DNvec2 _DNUI_vec2_lerp(DNvec2 a, DNvec2 b, float alpha)
{
	return {_DNUI_lerp(a.x, b.x, alpha), _DNUI_lerp(a.y, b.y, alpha)};
}

static inline DNvec3 _DNUI_vec3_lerp(DNvec3 a, DNvec3 b, float alpha)
{
	return {_DNUI_lerp(a.x, b.x, alpha), _DNUI_lerp(a.y, b.y, alpha), _DNUI_lerp(a.z, b.z, alpha)};
}

static inline DNvec4 _DNUI_vec4_lerp(DNvec4 a, DNvec4 b, float alpha)
{
	return {_DNUI_lerp(a.x, b.x, alpha), _DNUI_lerp(a.y, b.y, alpha), _DNUI_lerp(a.z, b.z, alpha), _DNUI_lerp(a.w, b.w, alpha)};
}

bool DNUItransition::update(float dt, DNUIelement* element, DNvec2 parentSize, DNvec2 size)
{
	//wait for delay:
	//---------------------------------
	if(delay > 0.0f)
	{
		delay -= dt;
		return true;
	}

	//update alpha:
	//---------------------------------
	alpha += dt / time;
	if(alpha > 1.0f)
		alpha = 1.0f;

	bool finished = alpha == 1.0f;

	//calculate corrected alpha (allows for different interpolation types):
	//---------------------------------
	float correctedAlpha;
	switch(interpolateType)
	{
	case DNUItransition::QUADRATIC:
		correctedAlpha = 1.0f - (1.0f - alpha) * (1.0f - alpha);
		break;
	case DNUItransition::CUBIC:
		correctedAlpha = 1.0f - (1.0f - alpha) * (1.0f - alpha) * (1.0f - alpha);
		break;
	case DNUItransition::EXPONENTIAL:
		correctedAlpha = alpha == 1.0f ? 1.0f : 1.0f - powf(2.0f, -10.0f * alpha);
		break;
	case DNUItransition::LINEAR:
	default:
		correctedAlpha = alpha;
		break;
	}

	//update components:
	//---------------------------------
	for(int i = 0; i < components.size(); i++)
	{
		Component comp = components[i];
		char* ptr = (char*)element + comp.offset;

		switch(comp.type)
		{
		case DNUItransition::Component::COORDINATE:
		{
			float pS = comp.coordinate.x ? parentSize.x : parentSize.y; //parent size
			float s  = comp.coordinate.x ? size.x : size.y; //size

			float org = comp.coordinate.original.calc_render_pos(0.0f, pS, s);
			float tar = comp.coordinate.target.calc_render_pos(0.0f, pS, s);

			*(DNUIcoordinate*)ptr = finished ? comp.coordinate.target : DNUIcoordinate(DNUIcoordinate::PIXELS, _DNUI_lerp(org, tar, correctedAlpha), DNUIcoordinate::CENTER_CENTER);
			break;
		}
		case DNUItransition::Component::DIMENSION:
		{
			float pS = comp.dimension.w ? parentSize.x : parentSize.y; //parent size
			float oS = comp.dimension.w ? size.y : size.x; //other size

			float org = comp.dimension.original.type == DNUIdimension::ASPECT ? oS * comp.dimension.original.aspectRatio : comp.dimension.original.calc_render_size(pS);
			float tar = comp.dimension.target.type   == DNUIdimension::ASPECT ? oS * comp.dimension.target.aspectRatio   : comp.dimension.target.calc_render_size(pS);

			*(DNUIdimension*)ptr = finished ? comp.dimension.target : DNUIdimension(DNUIdimension::PIXELS, _DNUI_lerp(org, tar, correctedAlpha));
			break;
		}
		case DNUItransition::Component::FLOAT:
			*(float*)ptr = finished ? comp.floating.target : _DNUI_lerp(comp.floating.original, comp.floating.target, correctedAlpha);
			break;
		case DNUItransition::Component::VEC2:
			*(DNvec2*)ptr = finished ? comp.vector2.target : _DNUI_vec2_lerp(comp.vector2.original, comp.vector2.target, correctedAlpha);
			break;
		case DNUItransition::Component::VEC3:
			*(DNvec3*)ptr = finished ? comp.vector3.target : _DNUI_vec3_lerp(comp.vector3.original, comp.vector3.target, correctedAlpha);
			break;
		case DNUItransition::Component::VEC4:
			*(DNvec4*)ptr = finished ? comp.vector4.target : _DNUI_vec4_lerp(comp.vector4.original, comp.vector4.target, correctedAlpha);
			break;
		default:
			break;
		}
	}

	//return whether the transition has finished:
	//---------------------------------
	return !finished;
}