#include "element.hpp"
extern "C"
{
	#include "render.h"
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
	for(int i = 0; i < children.size(); i++)
		children[i]->update(dt, parentPos, parentSize);
}

void DNUIelement::render()
{
	for(int i = 0; i < children.size(); i++)
		children[i]->render();
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

	DNUIelement::update(dt, renderPos, renderSize);
}

void DNUIbox::render()
{
	DNUI_draw_rect(texture, renderPos, renderSize, 0.0f, color, cornerRadius);
	DNUIelement::render();
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