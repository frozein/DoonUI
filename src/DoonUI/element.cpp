#include "element.hpp"
extern "C"
{
	#include "render.h"
	#include "math/vector.h"
}

//--------------------------------------------------------------------------------------------------------------------------------//
//HELPER FUNCTIONS:

//calculates the position an element should be rendered at
DNvec2 _DNUI_calc_render_pos(DNvec2 parentPos, DNvec2 parentSize, DNvec2 size, DNUIcoordinate x, DNUIcoordinate y)
{
	return {x.calc_render_pos(parentPos.x, parentSize.x, size.x), y.calc_render_pos(parentPos.y, parentSize.y, size.y)};
}

//calculates the size an element should be rendered at
DNvec2 _DNUI_calc_render_size(DNvec2 parentSize, DNUIdimension width, DNUIdimension height)
{
	if(width.type == DNUIdimension::ASPECT)
	{
		float h = height.calc_render_size(parentSize.y);
		return {h * width.aspectRatio, h};
	}
	else if(height.type == DNUIdimension::ASPECT)
	{
		float w = width.calc_render_size(parentSize.x);
		return {w, w * height.aspectRatio};
	}
	else
		return {width.calc_render_size(parentSize.x), height.calc_render_size(parentSize.y)};
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
	float requiredWidth = width.calc_render_size(parentSize.x);
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