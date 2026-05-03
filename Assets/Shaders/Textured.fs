#version 330 core

in vec2 TexCoord;
in vec3 Color;

out vec4 FragColor;

uniform sampler2D uTexture;
uniform vec2 uTiling = vec2(1.0, 1.0);

void main()
{
	FragColor = vec4(Color, 1.0) * texture(uTexture, TexCoord * uTiling);
}