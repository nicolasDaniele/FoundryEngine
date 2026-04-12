#version 330 core
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;

uniform mat4 vp;

out vec3 vertColor;

void main()
{
    vertColor = inColor;
    gl_Position = vp * vec4(inPos, 1.0);
}