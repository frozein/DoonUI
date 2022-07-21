#include "element.hpp"
extern "C"
{
	#include "render.h"
	#include "math/vector.h"
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

void _DNUI_lerp_pos(DNUIcoordinate* x, DNUIcoordinate* y, DNvec2 parentSize, DNvec2 size, float alpha, DNUIcoordinate aX, DNUIcoordinate aY, DNUIcoordinate bX, DNUIcoordinate bY)
{
	DNvec2 posA = _DNUI_calc_render_pos({0.0f, 0.0f}, parentSize, size, aX, aY);
	DNvec2 posB = _DNUI_calc_render_pos({0.0f, 0.0f}, parentSize, size, bX, bY);
	DNvec2 pos = DN_vec2_lerp(posA, posB, alpha);

	*x = DNUIcoordinate(DNUIcoordinate::PIXELS, pos.x, DNUIcoordinate::CENTER_CENTER);
	*y = DNUIcoordinate(DNUIcoordinate::PIXELS, pos.y, DNUIcoordinate::CENTER_CENTER);
}

void _DNUI_lerp_size(DNUIdimension* w, DNUIdimension* h, DNvec2 parentSize, float alpha, DNUIdimension aW, DNUIdimension aH, DNUIdimension bW, DNUIdimension bH)
{
	DNvec2 sizeA = _DNUI_calc_render_size(parentSize, aW, aH);
	DNvec2 sizeB = _DNUI_calc_render_size(parentSize, bW, bH);
	DNvec2 size = DN_vec2_lerp(sizeA, sizeB, alpha);

	*w = DNUIdimension(DNUIdimension::PIXELS, size.x);
	*h = DNUIdimension(DNUIdimension::PIXELS, size.y);
}

float _DNUI_lerp(float a, float b, float alpha)
{
	return a + alpha * (b - a);
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

DNUIanimData::DNUIanimData()
{
	a = 0.0f;
	speed = 1.0f;
	repeat = false;
	interpolateType = DNUIanimData::LINEAR;
}

DNUIanimData::DNUIanimData(float spd, bool rpt, DNUIanimData::InterpolationType interpolateTyp)
{
	a = 0.0f;
	speed = spd;
	repeat = rpt;
	interpolateType = interpolateTyp;
}

float DNUIanimData::update_alpha(float dt)
{
	a += dt * speed;

	if(a > 1.0f)
		a = 1.0f;
	else if(a < 0.0f)
		a = 0.0f;

	switch(interpolateType)
	{
	case DNUIanimData::QUADRATIC:
		return 1.0f - (1.0f - a) * (1.0f - a);
	case DNUIanimData::CUBIC:
		return 1.0f - (1.0f - a) * (1.0f - a) * (1.0f - a);
	case DNUIanimData::EXPONENTIAL:
		return a == 1.0f ? 1.0f : 1.0f - powf(2.0f, -10.0f * a);
	case DNUIanimData::LINEAR:
	default:
		return a;
	}
}

void DNUIanimData::reset()
{
	a = 0.0f;
}

//--------------------------------------------------------------------------------------------------------------------------------//

DNUIelement::DNUIelement()
{

}

DNUIelement::~DNUIelement()
{
	for(int i = 0; i < children.size(); i++)
		delete children[i];
}

void DNUIelement::update(float dt, DNvec2 parentPos, DNvec2 parentSize)
{
	for(int i = 0; i < children.size(); i++)
		children[i]->update(dt, parentPos, parentSize);
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

//--------------------------------------------------------------------------------------------------------------------------------//

DNUIbox::AnimState::AnimState()
{
	xPos = DNUIcoordinate();
	yPos = DNUIcoordinate();
	width  = DNUIdimension();
	height = DNUIdimension();

	color = {1.0f, 1.0f, 1.0f, 1.0f};
	angle = 0.0f;
	cornerRadius = 0.0f;
}

DNUIbox::AnimState::AnimState(DNUIcoordinate x, DNUIcoordinate y, DNUIdimension w, DNUIdimension h, DNvec4 col, float cornerRad, float agl)
{
	xPos = x;
	yPos = y;
	width  = w;
	height = h;

	color = col;
	angle = agl;
	cornerRadius = cornerRad;
}

DNUIbox::AnimState DNUIbox::AnimState::lerp(DNUIbox::AnimState a, DNUIbox::AnimState b, float alpha, DNvec2 parentSize)
{
	DNUIbox::AnimState res;

	_DNUI_lerp_size(&res.width, &res.height, parentSize, alpha, a.width, a.height, b.width, b.height);
	_DNUI_lerp_pos(&res.xPos, &res.yPos, parentSize, {res.width.pixelSize, res.height.pixelSize}, alpha, a.xPos, a.yPos, b.xPos, b.yPos);

	res.color = DN_vec4_lerp(a.color, b.color, alpha);
	res.cornerRadius = _DNUI_lerp(a.cornerRadius, b.cornerRadius, alpha);
	res.angle = _DNUI_lerp(a.angle, b.angle, alpha);

	return res;
}

DNUIbox::DNUIbox()
{
	currentState = AnimState();
	targetState = currentState;
	animData = DNUIanimData();

	texture = -1;
}

DNUIbox::DNUIbox(AnimState state, int tex)
{
	currentState = state;
	targetState = currentState;
	animData = DNUIanimData();

	texture = tex;
}

void DNUIbox::update(float dt, DNvec2 parentPos, DNvec2 parentSize)
{
	float a = animData.update_alpha(dt);

	renderState = DNUIbox::AnimState::lerp(currentState, targetState, a, parentSize);
	renderSize = _DNUI_calc_render_size(parentSize, renderState.width, renderState.height);
	renderPos = _DNUI_calc_render_pos(parentPos, parentSize, renderSize, renderState.xPos, renderState.yPos);

	if(a >= 1.0f)
	{
		animData.reset();
		if(animData.repeat)
		{
			AnimState temp = currentState;
			currentState = targetState;
			targetState = temp;
		}
		else
			currentState = targetState;
	}

	DNUIelement::update(dt, renderPos, renderSize);
}

void DNUIbox::render()
{
	DNUI_draw_rect(texture, renderPos, renderSize, renderState.angle, renderState.color, renderState.cornerRadius);
	DNUIelement::render();
}

void DNUIbox::start_anim(DNUIanimData data, AnimState state)
{
	animData = data;
	targetState = state;
}

//--------------------------------------------------------------------------------------------------------------------------------//

DNUIbutton::DNUIbutton() : DNUIbox::DNUIbox()
{
	button_callback = nullptr;
	callbackID = 0;
}

DNUIbutton::DNUIbutton(AnimState state, void (*buttonCallback)(int), int id, int tex) : DNUIbox::DNUIbox(state, tex)
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
	xPos = DNUIcoordinate();
	yPos = DNUIcoordinate();
	width  = DNUIdimension();

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

	DNvec2 renderSize = DNUI_string_render_size(text.c_str(), font, renderScale, renderW);
	renderPos = _DNUI_calc_render_pos(parentPos, parentSize, renderSize, xPos, yPos);

	DNUIelement::update(dt, renderPos, renderSize);
}

void DNUItext::render()
{
	if(font >= 0)
		DNUI_draw_string(text.c_str(), font, renderPos, renderScale, renderW, align, color, thickness, softness, outlineColor, outlineThickness, outlineSoftness);

	DNUIelement::render();
}