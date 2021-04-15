#pragma once
#include <GL/glew.h>
#include "Texture.h"
#include "Enums.h"
#include <glm/detail/type_vec3.hpp>
#include "BaseObject.h"

class TriplanarMesh : public BaseObject
{
public:
	TriplanarMesh();
	~TriplanarMesh();

	GLuint GetVAO(int index) const;
	GLuint GetVBO(int index) const;
	void UpdateVao(int index, int triCount);

	void Update(GLfloat deltaTime) override;
	void Render(Shader& shader, bool tesselate) const;

	GLsizei GetTriCount(int index) const;
	GLuint GetVaoCount() const;

private:
	GLuint* m_vbo;
	GLuint* m_vao;
	GLsizei* m_triCount;
	GLuint m_vaoCount;
	glm::vec3 m_color;
	ColorBlendMode m_colorMode;
	NormalBlendMode m_normalMode;
	Texture* m_texture;
	Texture* m_normalMap;
	Texture* m_displacementMap;
};

