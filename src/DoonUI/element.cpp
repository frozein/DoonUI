#include "element.hpp"
extern "C"
{
	#include "render.h"
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

//calculates the position an element should be rendered at for one dimension
float _DNUI_calc_pos_one_dimension(float parentPos, float parentSize, float size, DNUIcoordinate coordinate)
{
	float pos = 0.0f;

	//find position relative to parent:
	if(coordinate.type == DNUIcoordinate::PIXELS)
		switch(coordinate.center)
		{
		case DNUIcoordinate::CENTER_CENTER:
		{
			pos = parentPos + coordinate.pixelPos;
			break;
		}
		case DNUIcoordinate::CENTER_MAX:
		{
			pos = parentPos + parentSize * 0.5f - coordinate.pixelPos;
			break;
		}
		case DNUIcoordinate::CENTER_MIN:
		{
			pos = parentPos - parentSize * 0.5f + coordinate.pixelPos;
		}
		}
	else if (coordinate.type == DNUIcoordinate::RELATIVE)
		pos =  parentPos + parentSize * (coordinate.relativePos - 0.5f);

	//account for centering:
	if(coordinate.center == DNUIcoordinate::CENTER_MIN)
		pos += size * 0.5f;
	else if(coordinate.center == DNUIcoordinate::CENTER_MAX)
		pos -= size * 0.5f;

	//return:
	return pos;
}

//calculates the position an element should be rendered at
DNvec2 _DNUI_calc_render_pos(DNvec2 parentPos, DNvec2 parentSize, DNvec2 size, DNUIcoordinate x, DNUIcoordinate y)
{
	return {_DNUI_calc_pos_one_dimension(parentPos.x, parentSize.x, size.x, x), _DNUI_calc_pos_one_dimension(parentPos.y, parentSize.y, size.y, y)};
}

//calculates the size an element should be rendered at for one dimension
float _DNUI_calc_size_one_dimension(float parentSize, DNUIdimension dimension)
{
	switch(dimension.type)
	{
	case DNUIdimension::PIXELS:
		return dimension.pixelSize;
	case DNUIdimension::RELATIVE:
		return parentSize * dimension.relativeSize;
	case DNUIdimension::SPACE:
		return parentSize - dimension.emptyPixels;
	default:
		return parentSize;
	}
}

//calculates the size an element should be rendered at
DNvec2 _DNUI_calc_render_size(DNvec2 parentSize, DNUIdimension width, DNUIdimension height)
{
	if(width.type == DNUIdimension::ASPECT)
	{
		float h = _DNUI_calc_size_one_dimension(parentSize.y, height);
		return {h * width.aspectRatio, h};
	}
	else if(height.type == DNUIdimension::ASPECT)
	{
		float w = _DNUI_calc_size_one_dimension(parentSize.x, width);
		return {w, w * height.aspectRatio};
	}
	else
		return {_DNUI_calc_size_one_dimension(parentSize.x, width), _DNUI_calc_size_one_dimension(parentSize.y, height)};
}

float _DNUI_lerp(float a, float b, float alpha)
{
	return a + alpha * (b - a);
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

			float org = _DNUI_calc_pos_one_dimension(0.0f, pS, s, comp.coordinate.original);
			float tar = _DNUI_calc_pos_one_dimension(0.0f, pS, s, comp.coordinate.target);

			*(DNUIcoordinate*)ptr = finished ? comp.coordinate.target : DNUIcoordinate(DNUIcoordinate::PIXELS, _DNUI_lerp(org, tar, correctedAlpha), DNUIcoordinate::CENTER_CENTER);
			break;
		}
		case DNUItransition::Component::DIMENSION:
		{
			float pS = comp.dimension.w ? parentSize.x : parentSize.y; //parent size
			float oS = comp.dimension.w ? size.y : size.x; //other size

			float org = comp.dimension.original.type == DNUIdimension::ASPECT ? oS * comp.dimension.original.aspectRatio : _DNUI_calc_size_one_dimension(pS, comp.dimension.original);
			float tar = comp.dimension.target.type   == DNUIdimension::ASPECT ? oS * comp.dimension.target.aspectRatio   : _DNUI_calc_size_one_dimension(pS, comp.dimension.target  );

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

//--------------------------------------------------------------------------------------------------------------------------------//

DNUIelement::DNUIelement()
{

}

DNUIelement::DNUIelement(DNUIcoordinate x, DNUIcoordinate y, DNUIdimension w, DNUIdimension h)
{
	xPos = x;
	yPos = y;
	width = w;
	height = h;
}

DNUIelement::~DNUIelement()
{
	for(int i = 0; i < children.size(); i++)
		delete children[i];
}

void DNUIelement::update(float dt, DNvec2 parentPos, DNvec2 parentSize)
{
	if(activeTransition)
		activeTransition = transition.update(dt, this, parentSize, renderSize);

	for(int i = 0; i < children.size(); i++)
		children[i]->update(dt, renderPos, renderSize);
}

void DNUIelement::render()
{
	for(int i = 0; i < children.size(); i++)
		children[i]->render();
}

void DNUIelement::handle_event(DNUIevent event)
{
	for(int i = 0; i < children.size(); i++)
		children[i]->handle_event(event);
}

void DNUIelement::set_transition(DNUItransition trans, float delay)
{
	activeTransition = true;
	transition = trans;
	transition.init(this, delay);
}

//--------------------------------------------------------------------------------------------------------------------------------//

DNUIbox::DNUIbox()
{
	color = {1.0f, 1.0f, 1.0f, 1.0f};
	cornerRadius = 10.0f;
	texture = -1;
}

DNUIbox::DNUIbox(DNUIcoordinate x, DNUIcoordinate y, DNUIdimension w, DNUIdimension h, DNvec4 col, float cornerRad, int tex) : DNUIelement::DNUIelement(x, y, w, h)
{
	color = col;
	cornerRadius = cornerRad;
	texture = tex;
}

void DNUIbox::update(float dt, DNvec2 parentPos, DNvec2 parentSize)
{
	renderSize = _DNUI_calc_render_size(parentSize, width, height);
	renderPos = _DNUI_calc_render_pos(parentPos, parentSize, renderSize, xPos, yPos);

	DNUIelement::update(dt, parentPos, parentSize);
}

void DNUIbox::render()
{
	DNUI_draw_rect(texture, renderPos, renderSize, 0.0f, color, cornerRadius);
	DNUIelement::render();
}

//--------------------------------------------------------------------------------------------------------------------------------//

DNUIbutton::DNUIbutton() : DNUIbox::DNUIbox()
{
	button_callback = nullptr;
	callbackID = 0;
}

DNUIbutton::DNUIbutton(DNUIcoordinate x, DNUIcoordinate y, DNUIdimension w, DNUIdimension h, DNvec4 col, void (*buttonCallback)(int), int id, float cornerRad, int tex) : DNUIbox::DNUIbox(x, y, w, h, col, cornerRad, tex)
{
	button_callback = buttonCallback;
	callbackID = id;
}

void DNUIbutton::set_mouse_state(DNvec2 pos, bool pressed)
{
	mousePos = pos;
	mousePressed = pressed;
}

void DNUIbutton::update(float dt, DNvec2 parentPos, DNvec2 parentSize)
{
	if(mousePos.x > renderPos.x - renderSize.x * 0.5f && mousePos.x < renderPos.x + renderSize.x * 0.5f &&
	   mousePos.y > renderPos.y - renderSize.y * 0.5f && mousePos.y < renderPos.y + renderSize.y * 0.5f)
	{
		if(mousePressed)
		{
			//pressed anim state
		}
		else
		{
			//hovered anim state
		}
	}
	else
	{
		//base anim state
	}

	DNUIbox::update(dt, parentPos, parentSize);
}

void DNUIbutton::handle_event(DNUIevent event)
{
	if(event.type == DNUIevent::MOUSE_RELEASE)
	{
		if(button_callback != nullptr)
			button_callback(callbackID);
	}

	DNUIelement::handle_event(event);
}

//--------------------------------------------------------------------------------------------------------------------------------//

DNUItext::DNUItext() : DNUIelement::DNUIelement()
{
	height = DNUIdimension(DNUIdimension::RELATIVE, 1.0f);

	text = "Hello World!";
	color = {1.0f, 1.0f, 1.0f};
	font = -1;
	scale = 1.0f;
	lineWrap = 0.0f;
	align = 0;
	thickness = 0.5f;
	softness = 0.05f;
	outlineColor = {0.0f, 0.0f, 0.0f, 1.0f};
	outlineThickness = 1.0f;
	outlineSoftness = 0.0f;
}

DNUItext::DNUItext(DNUIcoordinate x, DNUIcoordinate y, DNUIdimension size, std::string txt, int fnt, DNvec4 col, float scl, float lnW, int algn, float thick, float soft, DNvec4 outlineCol, float outlineThick, float outlineSoft)
{
	xPos = x;
	yPos = y;
	width = size;
	height = DNUIdimension(DNUIdimension::RELATIVE, 1.0f);

	text = txt;
	color = col;
	font = fnt;
	scale = scl;
	lineWrap = lnW;
	align = algn;
	thickness = thick;
	softness = soft;
	outlineColor = outlineCol;
	outlineThickness = outlineThick;
	outlineSoftness = outlineSoft;
}

void DNUItext::update(float dt, DNvec2 parentPos, DNvec2 parentSize)
{
	float requiredWidth = _DNUI_calc_size_one_dimension(parentSize.x, width);
	if(lineWrap <= 0.0f)
	{
		renderScale = requiredWidth / DNUI_string_render_size(text.c_str(), font, 1.0f, 0.0f).x;
		renderW = 0.0f;
	}
	else
	{
		renderW = requiredWidth;

		if(scale <= 0.0f)
			renderScale = requiredWidth / lineWrap;
		else
			renderScale = scale;
	}

	renderSize = DNUI_string_render_size(text.c_str(), font, renderScale, renderW);
	renderPos = _DNUI_calc_render_pos(parentPos, parentSize, renderSize, xPos, yPos);

	DNUIelement::update(dt, parentPos, parentSize);
}

void DNUItext::render()
{
	if(font >= 0)
		DNUI_draw_string(text.c_str(), font, renderPos, renderScale, renderW, align, color, thickness, softness, outlineColor, outlineThickness, outlineSoftness);

	DNUIelement::render();
}