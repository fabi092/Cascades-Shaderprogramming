#include "Texture.h"
#include <Soil/SOIL.h>
#include <iostream>
#include "Global.h"


Texture::Texture()
{
	glGenTextures(1, &m_id);
}

Texture::Texture(GLchar* texturePath)
{
	{
		struct stat buffer;
		if (stat(texturePath, &buffer) != 0)
			printf("ERROR::TEXTURE::FILE_NOT_FOUND \"%s\"", texturePath);
	}

	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_2D, m_id); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

										// Set our texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load, create texture and generate mipmaps
	int width = 0, height = 0;
	unsigned char* image = SOIL_load_image(texturePath, &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
	glCheckError();
}

Texture::~Texture()
{
}

GLuint Texture::GetId() const
{
	return m_id;
}
