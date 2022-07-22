#include "utility.hpp"
#include "element.hpp"
extern "C"
{
	#include "math/vector.h"
}

//--------------------------------------------------------------------------------------------------------------------------------//

DNUIcoordinate::DNUIcoordinate()
{
	type = RELATIVE;
	relativePos = 1.0f;
	center = CENTER_CENTER;
}

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

DNUIdimension::DNUIdimension()
{
	type = RELATIVE;
	relativeSize = 1.0f;
}

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

DNUIevent::DNUIevent()
{
	type = DNUIevent::NONE;
}

DNUIevent::DNUIevent(DNUIevent::Type typ)
{
	type = typ;
}

//--------------------------------------------------------------------------------------------------------------------------------//

DNUItransition::DNUItransition()
{

}

DNUItransition::DNUItransition(float t, InterpolationType i)
{
	time = t;
	interpolateType = i;
}

void DNUItransition::add_target_x(DNUIcoordinate x)
{
	DNUItransition::Component comp = {};
	comp.type = DNUItransition::Component::COORDINATE;
	comp.offset = offsetof(DNUIelement, xPos);
	comp.coordinate.target = x;
	comp.coordinate.x = true;

	components.push_back(comp);
}

void DNUItransition::add_target_y(DNUIcoordinate y)
{
	DNUItransition::Component comp = {};
	comp.type = DNUItransition::Component::COORDINATE;
	comp.offset = offsetof(DNUIelement, yPos);
	comp.coordinate.target = y;
	comp.coordinate.x = false;

	components.push_back(comp);
}

void DNUItransition::add_target_w(DNUIdimension w)
{
	DNUItransition::Component comp = {};
	comp.type = DNUItransition::Component::DIMENSION;
	comp.offset = offsetof(DNUIelement, width);
	comp.dimension.target = w;
	comp.dimension.w = true;
	
	components.push_back(comp);
}

void DNUItransition::add_target_h(DNUIdimension h)
{
	DNUItransition::Component comp = {};
	comp.type = DNUItransition::Component::DIMENSION;
	comp.offset = offsetof(DNUIelement, height);
	comp.dimension.target = h;
	comp.dimension.w = false;
	
	components.push_back(comp);
}

void DNUItransition::add_target_color(DNvec4 col)
{
	add_target_vec4(col, offsetof(DNUIelement, color));
}

void DNUItransition::add_target_float(float target, size_t offset)
{
	DNUItransition::Component comp = {};
	comp.type = DNUItransition::Component::FLOAT;
	comp.offset = offset;
	comp.floating.target = target;

	components.push_back(comp);
}

void DNUItransition::add_target_vec2(DNvec2 target, size_t offset)
{
	DNUItransition::Component comp = {};
	comp.type = DNUItransition::Component::VEC2;
	comp.offset = offset;
	comp.vector2.target = target;

	components.push_back(comp);
}

void DNUItransition::add_target_vec3(DNvec3 target, size_t offset)
{
	DNUItransition::Component comp = {};
	comp.type = DNUItransition::Component::VEC3;
	comp.offset = offset;
	comp.vector3.target = target;

	components.push_back(comp);
}

void DNUItransition::add_target_vec4(DNvec4 target, size_t offset)
{
	DNUItransition::Component comp = {};
	comp.type = DNUItransition::Component::VEC4;
	comp.offset = offset;
	comp.vector4.target = target;

	components.push_back(comp);
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
		{
			components[i].coordinate.original = *(DNUIcoordinate*)ptr;
			break;
		}
		case DNUItransition::Component::DIMENSION:
		{
			components[i].dimension.original = *(DNUIdimension*)ptr;
			break;
		}
		case DNUItransition::Component::FLOAT:
		{
			components[i].floating.original = *(float*)ptr;
			break;
		}
		case DNUItransition::Component::VEC2:
		{
			components[i].vector2.original = *(DNvec2*)ptr;
			break;
		}
		case DNUItransition::Component::VEC3:
		{
			components[i].vector3.original = *(DNvec3*)ptr;
			break;
		}
		case DNUItransition::Component::VEC4:
		{
			components[i].vector4.original = *(DNvec4*)ptr;
			break;
		}
		default:
			break;
		}
	}
}

float _DNUI_lerp(float a, float b, float alpha)
{
	return a + alpha * (b - a);
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
			*(DNvec2*)ptr = finished ? comp.vector2.target : DN_vec2_lerp(comp.vector2.original, comp.vector2.target, correctedAlpha);
			break;
		case DNUItransition::Component::VEC3:
			*(DNvec3*)ptr = finished ? comp.vector3.target : DN_vec3_lerp(comp.vector3.original, comp.vector3.target, correctedAlpha);
			break;
		case DNUItransition::Component::VEC4:
			*(DNvec4*)ptr = finished ? comp.vector4.target : DN_vec4_lerp(comp.vector4.original, comp.vector4.target, correctedAlpha);
			break;
		default:
			break;
		}
	}

	//return whether the transition has finished:
	//---------------------------------
	return !finished;
}