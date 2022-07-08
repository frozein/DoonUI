#version 430 core

layout(location = 0) in vec2 inPos;
out vec2 texCoord;

uniform mat3 model;
uniform mat3 projection;

void main()
{
	vec3 pos = model * vec3(inPos, 1.0);
	gl_Position = vec4(pos.xy, 0.0, 1.0);
	texCoord = max(inPos, 0.0);
}