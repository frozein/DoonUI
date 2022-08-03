#ifndef DNUI_UTILITY_H
#define DNUI_UTILITY_H

#include <vector>
#include "math/common.h"

//--------------------------------------------------------------------------------------------------------------------------------//
//DIMENSION/POSITION STRUCTS:

//represents a position for a DNUIelement
struct DNUIcoordinate
{
	//determines how the element will be positioned
	enum Type
	{
		RELATIVE, //the element will be positioned relative to its parent's size. For example, a relativePos of 0.5 would place the element in the center of its parent
		PIXELS    //the element will be positioned relative to its parent's center, min, or max (depending on what center is set to), being offset by pixelPos pixels
	} type = RELATIVE;

	//determines at what point the element will be centered
	enum Center
	{
		CENTER_MIN,    //the element will be positioned centered at its minimum coordinate
		CENTER_CENTER, //the element will be positioned centered at its center
		CENTER_MAX     //the element will be positioned centered at its maximum coordinate
	} center = CENTER_CENTER;

	union
	{
		float relativePos = 0.5f;
		float pixelPos;
	};

	DNUIcoordinate() = default;
	DNUIcoordinate(DNUIcoordinate::Type type, float param, DNUIcoordinate::Center center);

	//returns the position relative to the screen, in pixels
	float calc_render_pos(float parentPos, float parentSize, float size);
};

struct DNUIdimension
{
	//determines how the element will be sized
	enum Type
	{
		RELATIVE, //the element will be sized relative to its parent's size. For example, a relativeSize of 0.5 would result in the element being half as large as it's parent
		PIXELS,   //the element will have a fixed pixel size
		ASPECT,   //the element's size will be relative to its size in the other dimension
		SPACE     //the element will be sized such that it takes up emptyPixels fewer pixels that it's parent does
	} type = RELATIVE;

	union
	{
		float relativeSize = 1.0f;
		float pixelSize;
		float aspectRatio;
		float emptyPixels;
	};

	DNUIdimension() = default;
	DNUIdimension(DNUIdimension::Type type, float param);

	//returns the size, in pixels
	float calc_render_size(float parentSize);
};

//--------------------------------------------------------------------------------------------------------------------------------//
//EVENT STRUCT:

struct DNUIevent
{
	enum Type
	{
		NONE,
		MOUSE_RELEASE
	} type = NONE;

	//data here

	DNUIevent() = default;
	DNUIevent(DNUIevent::Type type);
};

//--------------------------------------------------------------------------------------------------------------------------------//
//TRANSITION CLASS:

class DNUIelement;

//represents a transition for a UI element, facilitates interpolating to a new state
class DNUItransition
{
private:
	struct Component
	{
		enum DataType
		{
			COORDINATE,
			DIMENSION,
			FLOAT,
			VEC2,
			VEC3,
			VEC4
		} type;

		size_t offset;

		union
		{
			struct 
			{
				DNUIcoordinate original;
				DNUIcoordinate target;
				bool x;
			} coordinate;

			struct 
			{
				DNUIdimension original;
				DNUIdimension target;
				bool w;
			} dimension;

			struct
			{
				float original;
				float target;
			} floating;

			struct
			{
				DNvec2 original;
				DNvec2 target;
			} vector2;

			struct
			{
				DNvec3 original;
				DNvec3 target;
			} vector3;

			struct
			{
				DNvec4 original;
				DNvec4 target;
			} vector4;
		};
	};

	std::vector<Component> components;
	float alpha = 0.0f;
	float delay = 0.0f;

	//returns the index of the specified component, will push_back the vector and return the new index if an existing component was not found
	int get_component(Component::DataType type, size_t offset);

public:
	float time = 1000.0f; //how long the transition will take, in milliseconds

	//determines how the transition will be interpolated
	enum InterpolationType
	{
		LINEAR,
		QUADRATIC,
		CUBIC,
		EXPONENTIAL
	} interpolateType = LINEAR;

	DNUItransition() = default;
	DNUItransition(float time, InterpolationType interpolate);

	//sets the target x position
	void set_target_x(DNUIcoordinate x);
	//sets the target y position
	void set_target_y(DNUIcoordinate y);
	//sets the target width
	void set_target_w(DNUIdimension  w);
	//sets the target height
	void set_target_h(DNUIdimension  h);
	//sets the target alpha multiplier
	void set_target_alphamult(float a);

	//sets a target float value, use offsetof to determine the offset
	void set_target_float(float target, size_t offset);
	//sets a target vec2 value, use offsetof to determine the offset
	void set_target_vec2(DNvec2 target, size_t offset);
	//sets a target vec3 value, use offsetof to determine the offset
	void set_target_vec3(DNvec3 target, size_t offset);
	//sets a target vec4 value, use offsetof to determine the offset
	void set_target_vec4(DNvec4 target, size_t offset);

	//initializes the transition, call before calling update(). the animation will begin after delay() milliseconds
	void init(DNUIelement* element, float delay);
	//updates the transition
	bool update(float dt, DNUIelement* element, DNvec2 parentSize, DNvec2 size);
};

#endif