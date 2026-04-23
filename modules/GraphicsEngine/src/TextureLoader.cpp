#include <glad/glad.h>
#include <iostream>
#include <string>
#include "GraphicsEngine/TextureLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLuint TextureLoader::LoadTexture(std::string texFileName)
{
	int width, height, channels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* image = stbi_load(texFileName.c_str(), &width, &height, &channels, 0);
	GLuint texture;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	
	// TODO: Make all these parameters configurable
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	if (image)
	{
		if (channels == 1)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, image);
		if(channels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		if(channels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "[TextureLoader] Failed to load texture" << std::endl;
	
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image);

	return texture;
}