#version 330 core

in vec2 TexCoord;

out vec4 color;

uniform sampler2D Texture;
uniform vec2 uTiling = vec2(1.0, 1.0);

void main()
{
	color = texture(Texture, TexCoord * uTiling);
}