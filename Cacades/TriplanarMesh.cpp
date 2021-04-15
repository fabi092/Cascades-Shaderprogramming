#include "TriplanarMesh.h"
#include "Global.h"
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"


TriplanarMesh::TriplanarMesh() : BaseObject(glm::vec3(0)), m_triCount(nullptr), m_vaoCount(64), m_colorMode(ColorBlendMode::ColorOnly), m_normalMode(NormalBlendMode::NormalsOnly), m_texture(nullptr), m_normalMap(nullptr), m_displacementMap(nullptr)
{
	m_color = glm::vec3(1);

	m_triCount = new GLsizei[m_vaoCount];

	m_vao = new GLuint[m_vaoCount];
	glGenVertexArrays(m_vaoCount, m_vao);
	m_vbo = new GLuint[m_vaoCount];
	glGenBuffers(m_vaoCount, m_vbo);

	for (int i = 0; i < m_vaoCount; ++i)
	{
		glBindVertexArray(m_vao[i]);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo[i]);
		//glBufferData(GL_ARRAY_BUFFER, m_triCount[i] * 3 * sizeof(glm::vec3) * 2, nullptr, GL_STATIC_DRAW);
		glCheckError();
		// Position attribute
		glEnableVertexAttribArray(VS_IN_POSITION);
		glVertexAttribPointer(VS_IN_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) * 3, (GLvoid*)0);
		glEnableVertexAttribArray(VS_IN_NORMAL);
		glVertexAttribPointer(VS_IN_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) * 3, (GLvoid*)sizeof(glm::vec3));
		glEnableVertexAttribArray(VS_IN_UV);
		glVertexAttribPointer(VS_IN_UV, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) * 3, (GLvoid*)(2 * sizeof(glm::vec3)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glCheckError();
		glBindVertexArray(0);
	}

	m_texture = new Texture[3]{ Texture("textures/Lava_001_COLOR.png"), Texture("textures/Lava_001_COLOR.png") , Texture("textures/Lava_001_COLOR.png") };
	m_normalMap = new Texture[3]{ Texture("textures/Lava_001_NRM.png"), Texture("textures/Lava_001_NRM.png") , Texture("textures/Lava_001_NRM.png") };
	m_displacementMap = new Texture[3]{ Texture("textures/Lava_001_DISP.png"), Texture("textures/Lava_001_DISP.png") , Texture("textures/Lava_001_DISP.png") };
	//m_texture = new Texture[3]{ Texture("textures/Rough_Rock_022_COLOR.jpg"), Texture("textures/Rough_Rock_022_COLOR.jpg") , Texture("textures/Rough_Rock_022_COLOR.jpg") };
	//m_normalMap = new Texture[3]{ Texture("textures/Rough_Rock_022_NRM.jpg"), Texture("textures/Rough_Rock_022_NRM.jpg") , Texture("textures/Rough_Rock_022_NRM.jpg") };
	//m_displacementMap = new Texture[3]{ Texture("textures/Rough_Rock_022_DISP.png"), Texture("textures/Rough_Rock_022_DISP.png") , Texture("textures/Rough_Rock_022_DISP.png") };
}


TriplanarMesh::~TriplanarMesh()
{
	glDeleteVertexArrays(m_vaoCount, m_vao);
	glDeleteBuffers(m_vaoCount, m_vbo);
}

GLuint TriplanarMesh::GetVAO(int index) const
{
	return m_vao[index];
}

GLuint TriplanarMesh::GetVBO(int index) const
{
	return m_vbo[index];
}

void TriplanarMesh::UpdateVao(int index, int triCount)
{
	m_triCount[index] = triCount;
}

void TriplanarMesh::Update(GLfloat deltaTime)
{
}

void TriplanarMesh::Render(Shader& shader, bool tesselate) const
{
	GLint modelLoc = glGetUniformLocation(shader.Program, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(GetMatrix()));
	glCheckError();

	GLint colorModeLoc = glGetUniformLocation(shader.Program, "colorMode");
	glUniform1i(colorModeLoc, m_colorMode);
	glCheckError();

	GLint objectColorLoc = glGetUniformLocation(shader.Program, "objectColor");
	glUniform3fv(objectColorLoc, 1, glm::value_ptr(m_color));
	glCheckError();

	GLint normalModeLoc = glGetUniformLocation(shader.Program, "normalMode");
	glUniform1i(normalModeLoc, m_normalMode);
	glCheckError();

	for (int i = 0; i < 3; ++i)
	{
		int textureLoc = glGetUniformLocation(shader.Program, ("objectTexture[" + std::to_string(i) + "]").c_str());
		glActiveTexture(GL_TEXTURE1 + i * 3);
		glBindTexture(GL_TEXTURE_2D, m_texture[i].GetId());
		glUniform1i(textureLoc, 1 + i * 3);
		glCheckError();
		int normalLoc = glGetUniformLocation(shader.Program, ("normalMap[" + std::to_string(i) + "]").c_str());
		glActiveTexture(GL_TEXTURE2 + i * 3);
		glBindTexture(GL_TEXTURE_2D, m_texture[i].GetId());
		glUniform1i(normalLoc, 2 + i * 3);
		glCheckError();
		int displacementLoc = glGetUniformLocation(shader.Program, ("displacementMap[" + std::to_string(i) + "]").c_str());
		glActiveTexture(GL_TEXTURE3 + i * 3);
		glBindTexture(GL_TEXTURE_2D, m_texture[i].GetId());
		glUniform1i(displacementLoc, 3 + i * 3);
		glCheckError();
	}

	for (int i = 0; i < m_vaoCount; ++i)
	{
		glBindVertexArray(m_vao[i]);
		glCheckError();
		if (tesselate)
		{
			glPatchParameteri(GL_PATCH_VERTICES, 3);
			glDrawArrays(GL_PATCHES, 0, m_triCount[i] * 3);
		}
		else
			glDrawArrays(GL_TRIANGLES, 0, m_triCount[i] * 3);
		glCheckError();
		glBindVertexArray(0);
	}
}

GLsizei TriplanarMesh::GetTriCount(int index) const
{
	return m_triCount[index];
}

GLuint TriplanarMesh::GetVaoCount() const
{
	return m_vaoCount;
}
