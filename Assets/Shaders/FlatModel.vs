#version 330 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Color;

uniform mat4 vp;
uniform mat4 model;

out vec3 outColor;

void main()
{
	gl_Position = vp * model * vec4(Position, 1.0);
	outColor = Color;
}