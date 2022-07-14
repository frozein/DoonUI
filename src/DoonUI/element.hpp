#ifndef DNUI_ELEMENT_H
#define DNUI_ELEMENT_H

#include <vector>
#include "math/common.h"

//--------------------------------------------------------------------------------------------------------------------------------//
//DIMENSION/POSITION STRUCTS:

enum DNUIcoordinateType
{
	DNUI_COORDINATE_RELATIVE,
	DNUI_COORDINATE_PIXEL
};

enum DNUIdimensionType
{
	DNUI_DIMENSION_RELATIVE,
	DNUI_DIMENSION_PIXELS,
	DNUI_DIMENSION_ASPECT,
	DNUI_DIMENSION_SPACE
};

enum DNUIcenter
{
	DNUI_CENTER_MIN,
	DNUI_CENTER_CENTER,
	DNUI_CENTER_MAX
};

struct DNUIcoordinate
{
	DNUIcoordinate();
	DNUIcoordinate(DNUIcoordinateType type, float param, DNUIcenter center);

	DNUIcoordinateType type;
	DNUIcenter center;

	union
	{
		float relativePos;
		float pixelPos;
	};
};

struct DNUIdimension
{
	DNUIdimension();
	DNUIdimension(DNUIdimensionType type, float param);

	DNUIdimensionType type;

	union
	{
		float relativeSize;
		float pixelSize;
		float aspectRatio;
		float emptyPixels;
	};
};

//--------------------------------------------------------------------------------------------------------------------------------//
//ELEMENT CLASS:

class DNUIelement
{
protected:
	DNvec2 pixelPos;
	DNvec2 pixelSize;

	void calculate_pixel_pos(DNvec2 parentPos, DNvec2 parentSize); //calculate_size should be called first
	void calculate_pixel_size(DNvec2 parentSize);

public:
	std::vector<DNUIelement*> children;

	DNUIcoordinate xPos;
	DNUIcoordinate yPos;

	DNUIdimension width;
	DNUIdimension height;

	DNUIelement();
	DNUIelement(DNUIcoordinate x, DNUIcoordinate y, DNUIdimension w, DNUIdimension h);
	~DNUIelement();

	virtual void update(float dt, DNvec2 parentPos, DNvec2 parentSize);
	virtual void render();
};

//--------------------------------------------------------------------------------------------------------------------------------//
//DEFAULT ELEMENTS:

class DNUIbox : public DNUIelement
{
public:
	DNvec4 color;
	float cornerRadius;
	int texture;

	DNUIbox();
	DNUIbox(DNUIcoordinate x, DNUIcoordinate y, DNUIdimension w, DNUIdimension h, DNvec4 col, float cornerRad, int tex);

	void update(float dt, DNvec2 parentPos, DNvec2 parentSize);
	void render();
};

#endif