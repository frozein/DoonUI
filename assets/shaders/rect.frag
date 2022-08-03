#version 430 core

out vec4 FragColor;

uniform vec4 color; 	 //the color of the element
uniform vec2 size; 		 //the size, in pixels
uniform float cornerRad; //the radius of the corner rounding

uniform vec4 outlineColor;
uniform float outlineThickness;

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

	//check distance (from https://iquilezles.org/articles/distfunctions2d/):
	//---------------------------------
	vec2 d = abs((texCoord - 0.5) * size) - (size * 0.5 - cornerRad);
	float dist = length(max(d, 0.0)) + min(max(d.x, d.y), 0.0) - cornerRad;

	//check if should be outlined:
	//---------------------------------
	float outlineA = smoothstep(-outlineThickness, -outlineThickness + 2.0, dist);
	finalColor = mix(finalColor, outlineColor, outlineA);

	finalColor.a *= smoothstep(1.0, -1.0, dist);

	//return:
	//---------------------------------
	FragColor = finalColor;
}