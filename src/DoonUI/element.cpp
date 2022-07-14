#include "element.hpp"
extern "C"
{
	#include "render.h"
}

//--------------------------------------------------------------------------------------------------------------------------------//

DNUIcoordinate::DNUIcoordinate()
{
	type = DNUI_COORDINATE_RELATIVE;
	relativePos = 1.0f;
	center = DNUI_CENTER_CENTER;
}

DNUIcoordinate::DNUIcoordinate(DNUIcoordinateType t, float param, DNUIcenter c)
{
	type = t;
	relativePos = param; //they are all single floats so this is fine
	center = c;
};

DNUIdimension::DNUIdimension()
{
	type = DNUI_DIMENSION_RELATIVE;
	relativeSize = 1.0f;
}

DNUIdimension::DNUIdimension(DNUIdimensionType t, float param)
{
	type = t;
	relativeSize = param; //they are all single floats so this is fine
}

//--------------------------------------------------------------------------------------------------------------------------------//

float _DNUI_calc_pos_one_dimension(float parentPos, float parentSize, float size, DNUIcoordinate coordinate)
{
	float pos;

	switch(coordinate.type)
	{
	case DNUI_COORDINATE_PIXEL:
		switch(coordinate.center)
		{
		case DNUI_CENTER_CENTER:
		{
			pos = parentPos + coordinate.pixelPos;
			break;
		}
		case DNUI_CENTER_MAX:
		{
			pos = parentPos + parentSize * 0.5f - coordinate.pixelPos;
			break;
		}
		case DNUI_CENTER_MIN:
		{
			pos = parentPos - parentSize * 0.5f + coordinate.pixelPos;
		}
		}
		break;
	case DNUI_COORDINATE_RELATIVE:
		pos =  parentPos + parentSize * (coordinate.relativePos - 0.5f);
		break;
	}

	switch(coordinate.center)
	{
	case DNUI_CENTER_CENTER:
		return pos;
	case DNUI_CENTER_MIN:
		return pos + size * 0.5f;
	case DNUI_CENTER_MAX:
		return pos - size * 0.5f;
	default:
		return 0.0f; //to make compiler shut up
	}
}

void DNUIelement::calculate_pixel_pos(DNvec2 parentPos, DNvec2 parentSize)
{
	pixelPos = {_DNUI_calc_pos_one_dimension(parentPos.x, parentSize.x, pixelSize.x, xPos), _DNUI_calc_pos_one_dimension(parentPos.y, parentSize.y, pixelSize.y, yPos)};
}

float _DNUI_calc_size_one_dimension(float parentSize, DNUIdimension dimension)
{
	switch(dimension.type)
	{
	case DNUI_DIMENSION_PIXELS:
		return dimension.pixelSize;
	case DNUI_DIMENSION_RELATIVE:
		return parentSize * dimension.relativeSize;
	case DNUI_DIMENSION_SPACE:
		return parentSize - dimension.emptyPixels;
	default:
		return parentSize;
	}
}

void DNUIelement::calculate_pixel_size(DNvec2 parentSize)
{
	if(width.type == DNUI_DIMENSION_ASPECT)
	{
		float h = _DNUI_calc_size_one_dimension(parentSize.y, height);
		pixelSize = {h * width.aspectRatio, h};
	}
	else if(height.type == DNUI_DIMENSION_ASPECT)
	{
		float w = _DNUI_calc_size_one_dimension(parentSize.x, width);
		pixelSize = {w, w * height.aspectRatio};
	}
	else
		pixelSize = {_DNUI_calc_size_one_dimension(parentSize.x, width), _DNUI_calc_size_one_dimension(parentSize.y, height)};
}

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
		children[i]->update(dt, pixelPos, pixelSize);
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
	calculate_pixel_size(parentSize);
	calculate_pixel_pos(parentPos, parentSize);

	DNUIelement::update(dt, parentPos, parentSize);
}

void DNUIbox::render()
{
	DNUI_draw_rect(texture, pixelPos, pixelSize, 0.0f, color, cornerRadius);

	DNUIelement::render();
}