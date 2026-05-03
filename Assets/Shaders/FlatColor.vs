#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 2) in vec3 aColor;

out vec3 Color;

uniform mat4 vp;
uniform mat4 model;

void main()
{
	gl_Position = vp * model * vec4(aPosition, 1.0);
	Color = aColor;
}