#pragma once
#include <GL/glew.h>

class NoiseTexture
{
public:
	NoiseTexture(GLuint width, GLuint height, GLuint depth, GLuint octave);
	~NoiseTexture();

	GLuint GetId() const;

protected:
	GLfloat *m_data;
	GLuint m_id;
	const GLuint a = 1;
	const GLuint b = 2;

};

