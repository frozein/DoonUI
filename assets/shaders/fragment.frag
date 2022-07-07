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
	vec2 pixelPos = texCoord * size;
	vec2 testPos;
	bool test = true;
	if(pixelPos.x < cornerRad)
		testPos.x = cornerRad;
	else if(pixelPos.x > size.x - cornerRad)
		testPos.x = size.x - cornerRad;
	else
		test = false;

	if(pixelPos.y < cornerRad)
		testPos.y = cornerRad;
	else if(pixelPos.y > size.y - cornerRad)
		testPos.y = size.y - cornerRad;
	else
		test = false;

	if(test && distance(testPos, pixelPos) > cornerRad)
		discard;

	vec4 finalColor = color;
	if(useTex)
		finalColor *= texture(tex, texCoord);

	FragColor = finalColor;
}