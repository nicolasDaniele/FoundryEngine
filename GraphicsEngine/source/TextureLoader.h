#pragma once

class TextureLoader
{
public:
	TextureLoader() {}
	~TextureLoader() {}

	GLuint LoadTexture(std::string texFileName);
};