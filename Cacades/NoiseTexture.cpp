#include "NoiseTexture.h"
#include <glm/gtc/noise.hpp>
#include "Global.h"

NoiseTexture::NoiseTexture(GLuint width, GLuint height, GLuint depth, GLuint octave)
{
	--octave;

	m_data = new GLfloat[width * height * depth];

	float xFactor = 1.0f / (width - 1);
	float yFactor = 1.0f / (height - 1);
	float zFactor = 1.0f / (depth - 1);

	float freq = a * (1 << octave);//  glm::pow(2.0f, octave);
	float scale = 4 / std::powf(b, octave);

	for (int dep = 0; dep < depth; dep++)
	{
		float z = zFactor * dep;
		for (int row = 0; row < height; row++)
		{
			float y = yFactor * row;
			for (int col = 0; col < width; col++)
			{
				float x = xFactor * col;

				glm::vec3 p(x * freq, y * freq, z * freq);

				// Store in texture buffer
				m_data[(dep * width * height + row * width + col)] = glm::perlin(p, glm::vec3(freq)) * scale;;
			}
		}
	}

	glGenTextures(1, &m_id);

	glBindTexture(GL_TEXTURE_3D, m_id);
	glCheckError();
	glTexImage3D(GL_TEXTURE_3D, 0, GL_R16F, width, height, depth, 0, GL_RED, GL_FLOAT, m_data);
	glCheckError();
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glCheckError();
}

NoiseTexture::~NoiseTexture()
{
	delete[] m_data;
}

GLuint NoiseTexture::GetId() const
{
	return m_id;
}
