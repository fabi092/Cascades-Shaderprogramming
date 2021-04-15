#include "MeasuringTool.h"
#include "Box.h"
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"


MeasuringTool::MeasuringTool() : m_hits {
Model(glm::vec3(0, 0, 0), MakeQuat(0, 0, 0), Box::GetTris(glm::vec3(.1f)), 12, glm::vec3(1, 0, 0), NoNormals),
Model(glm::vec3(0, 0, 0), MakeQuat(0, 0, 0), Box::GetTris(glm::vec3(.1f)), 12, glm::vec3(1, 0, 0), NoNormals)
}
{
	glGenVertexArrays(1, &vao_);
	glGenBuffers(1, &vbo_);
	glBindVertexArray(vao_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(VS_IN_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(VS_IN_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


MeasuringTool::~MeasuringTool()
{
}

void MeasuringTool::SetHit(int index, glm::vec3 position)
{
	m_hits[index].SetPosition(position);
}

glm::vec3 MeasuringTool::GetHit(int index) const
{
	return m_hits[index].GetPosition();
}

float MeasuringTool::GetDistance() const
{
	return glm::length(m_hits[0].GetPosition() - m_hits[1].GetPosition());
}

void MeasuringTool::Render(Shader& shader) const
{
	m_hits[0].Render(shader);
	m_hits[1].Render(shader);

	glm::vec3 vertices[2] = {
		m_hits[0].GetPosition(),
		m_hits[1].GetPosition()
	};

	GLint modelLoc = glGetUniformLocation(shader.Program, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));
	glCheckError();

	GLint colorModeLoc = glGetUniformLocation(shader.Program, "colorMode");
	glUniform1i(colorModeLoc, ColorOnly);
	glCheckError();

	GLint objectColorLoc = glGetUniformLocation(shader.Program, "objectColor");
	glUniform3fv(objectColorLoc, 1, glm::value_ptr(glm::vec3(1,0,0)));
	glCheckError();

	GLint normalModeLoc = glGetUniformLocation(shader.Program, "normalMode");
	glUniform1i(normalModeLoc, NoNormals);
	glCheckError();

	glBindVertexArray(vao_);

	// Update content of VBO memory
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData
	glCheckError();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glCheckError();


	// Render quad
	glDrawArrays(GL_LINES, 0, 2);
	glCheckError();
	glBindVertexArray(0);
}

void MeasuringTool::Update(GLfloat deltaTime)
{

}
