#version 430 core

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inTexCoord;

out vec2 texCoord;
uniform mat3 modelProjection;

void main()
{
	vec3 pos = modelProjection * vec3(inPos, 1.0);
	gl_Position = vec4(pos.xy, 0.0, 1.0);
	texCoord = inTexCoord;
}