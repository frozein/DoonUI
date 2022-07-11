#version 430 core

in vec2 texCoord;
out vec4 FragColor;

uniform vec4 color;
uniform sampler2D textureAtlas;

void main()
{
	FragColor = vec4(1.0);//vec4(color.rgb, texture(textureAtlas, texCoord).r * color.a);
}