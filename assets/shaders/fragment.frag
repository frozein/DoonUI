#version 430 core

out vec4 FragColor;

uniform vec4 color; 	 //the color of the element
uniform vec2 size; 		 //the size, in pixels
uniform float cornerRad; //the radius of the corner rounding

uniform bool useTex; 	 //whether or not to sample a texture
uniform sampler2D tex; 	 //the texture to sample
in vec2 texCoord; 		 //the texture coordinate

void main()
{
	//set color:
	//---------------------------------
	vec4 finalColor = color;
	if(useTex)
		finalColor *= texture(tex, texCoord);

	//see if pixel is in corner:
	//---------------------------------
	vec2 pixelPos = texCoord * size;
	vec2 cornerPos;
	bool testCorner = true;
	
	if(pixelPos.x < cornerRad)
		cornerPos.x = cornerRad;
	else if(pixelPos.x > size.x - cornerRad)
		cornerPos.x = size.x - cornerRad;
	else
		testCorner = false;

	if(pixelPos.y < cornerRad)
		cornerPos.y = cornerRad;
	else if(pixelPos.y > size.y - cornerRad)
		cornerPos.y = size.y - cornerRad;
	else
		testCorner = false;

	//test if pixel is within radius:
	//---------------------------------
	const vec2 samplePoints[] = {vec2(-0.375, -0.375), vec2(-0.375, 0.375), vec2(0.375, -0.375), vec2(0.375, 0.375)}; //multisample to allow for smoother corners
	float mult = 1.0;

	for(int i = 0; i < 4; i++)
	{
		vec2 newPos = pixelPos + samplePoints[i];
		vec2 toCenter = cornerPos - newPos;
		if(testCorner && dot(toCenter, toCenter) > cornerRad * cornerRad)
			mult -= 0.25;
	}

	finalColor.a *= mult;

	//return:
	//---------------------------------
	FragColor = finalColor;
}