#version 430 core

in vec2 texCoord;
out vec4 FragColor;

uniform sampler2D textureAtlas;

uniform vec4 color;
uniform float scale;
uniform float thickness;
uniform float softness;

uniform vec4 outlineColor;
uniform float outlineThickness;
uniform float outlineSoftness;

void main()
{
	float dist = texture(textureAtlas, texCoord).r;
	
	float a = smoothstep(thickness - softness / scale, thickness + softness / scale, dist);
	float outlineA = smoothstep(outlineThickness - outlineSoftness / scale, outlineThickness + outlineSoftness / scale, dist);

	vec4 finalColor = mix(outlineColor, color, outlineA);
	FragColor = vec4(finalColor.rgb, finalColor.a * a);
}