#pragma once
#include "BaseObject.h"
#include "Enums.h"
#include <vector>

class Texture;
struct Triangle;

class Model : public BaseObject
{
public:
	Model(glm::vec3 position, glm::quat orientaton, Triangle* tris, GLsizei triCount, glm::vec3 color, NormalBlendMode normalMode = NormalsOnly);
	Model(glm::vec3 position, glm::quat orientaton, Triangle* tris, GLsizei triCount, glm::vec3 color, ColorBlendMode colorMode, Texture* texture, NormalBlendMode normalMode = NormalsOnly);
	Model(glm::vec3 position, glm::quat orientaton, Triangle* tris, GLsizei triCount, glm::vec3 color, NormalBlendMode normalMode, Texture* normalMap);
	Model(glm::vec3 position, glm::quat orientaton, Triangle* tris, GLsizei triCount, glm::vec3 color, ColorBlendMode colorMode, Texture* texture, NormalBlendMode normalMode, Texture* normalMap);
	Model(glm::vec3 position, glm::quat orientaton, Triangle* tris, GLsizei triCount, glm::vec3 color, ColorBlendMode colorMode, Texture* texture, NormalBlendMode normalMode, Texture* normalMap, DisplacementMode displacementMode, Texture* heightMap);
	virtual ~Model();

	void Update(GLfloat deltaTime) override;
	void Render(Shader& shader) const override;

	GLuint GetVAO() const
	{
		return m_vao;
	}

	bool IsVisible() const;
	void IsVisible(bool visibility);

protected:
	bool m_isVisible;
	GLuint m_vbo;
	GLuint m_vao;
	Triangle* m_tris;
	GLsizei m_triCount;
	glm::vec3 m_color;
	ColorBlendMode m_colorMode;
	NormalBlendMode m_normalMode;
	DisplacementMode m_displacementMode;
	Texture* m_texture;
	Texture* m_normalMap;
	Texture* m_displacementMap;
};

