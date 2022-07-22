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
		finalColor = vec4(1.0, 1.0, 1.0, texture(tex, texCoord).r);

	//check distance (from https://iquilezles.org/articles/distfunctions2d/):
	//---------------------------------
	vec2 d = abs((texCoord - 0.5) * size) - (size * 0.5 - cornerRad);
	float dist = length(max(d, 0.0)) + min(max(d.x, d.y), 0.0) - cornerRad;
	finalColor.a *= smoothstep(1.0, -1.0, dist);

	//return:
	//---------------------------------
	FragColor = finalColor;
}