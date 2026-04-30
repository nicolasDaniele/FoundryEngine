#version 330 core

layout (location = 0) in vec3 position;
layout (location = 2) in vec3 Color;
layout (location = 3) in vec2 texCoord;

out vec2 TexCoord;
out vec3 outColor;

uniform mat4 vp;
uniform mat4 model;

void main()
{
	gl_Position = vp * model *vec4(position, 1.0);
	TexCoord = texCoord;
	outColor = Color;
}