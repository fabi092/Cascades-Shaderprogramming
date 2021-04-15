#pragma once
#include <GL/glew.h>
#include <glm/detail/type_vec3.hpp>
#include <glm/detail/type_vec2.hpp>

struct Vertex
{
	Vertex(glm::vec3 pos, glm::vec3 normal, glm::vec2 uv);
	Vertex(glm::vec3 pos, glm::vec3 normal);
	Vertex(glm::vec3 pos);
	Vertex();

	static void ConfigVertexArrayObject(GLuint vao);

	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 UV;
	glm::vec3 Tangent;
};
