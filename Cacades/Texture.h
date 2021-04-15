#pragma once
#include <GL/glew.h>

class Texture
{
public:
	Texture();
	Texture(GLchar* texturePath);
	~Texture();

	GLuint GetId() const;

private:
	GLuint m_id;
};

