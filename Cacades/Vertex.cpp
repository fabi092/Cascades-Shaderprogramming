#include "Vertex.h"
#include "Global.h"

Vertex::Vertex(glm::vec3 pos, glm::vec3 normal, glm::vec2 uv) : Position(pos), Normal(normal), UV(uv)
{}

Vertex::Vertex(glm::vec3 pos, glm::vec3 normal) : Vertex(pos, normal, glm::vec2(0, 0))
{}

Vertex::Vertex(glm::vec3 pos) : Vertex(pos, glm::vec3(0, 0, 0))
{}

Vertex::Vertex() : Vertex(glm::vec3(0, 0, 0))
{}

void Vertex::ConfigVertexArrayObject(GLuint vao)
{
	glBindVertexArray(vao);
	// Position attribute
	glVertexAttribPointer(VS_IN_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	glEnableVertexAttribArray(VS_IN_POSITION);

	// Normal attribute
	glVertexAttribPointer(VS_IN_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(VS_IN_NORMAL);

	// Texture attribute
	glVertexAttribPointer(VS_IN_UV, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(VS_IN_UV);

	// Texture attribute
	glVertexAttribPointer(VS_IN_TANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(VS_IN_TANGENT);

	glBindVertexArray(0);
	glCheckError();
}