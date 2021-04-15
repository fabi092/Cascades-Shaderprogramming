#include "Triangle.h"


Triangle::Triangle(Vertex v0, Vertex v1, Vertex v2)
{
	vertices_[0] = v0;
	vertices_[1] = v1;
	vertices_[2] = v2;

	glm::vec3 edge1 = v1.Position - v0.Position;
	glm::vec3 edge2 = v2.Position - v0.Position;
	glm::vec2 deltaUV1 = v1.UV - v0.UV;
	glm::vec2 deltaUV2 = v2.UV - v0.UV;

	GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	glm::vec3 tangent;
	tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	tangent = glm::normalize(tangent);

	for (int i = 0; i < 3; i++)
	{
		vertices_[i].Tangent = tangent;
	}
}

Triangle::Triangle(Vertex vertices[3]) : Triangle(vertices[0], vertices[1], vertices[2])
{}

Vertex& Triangle::GetVertex(int index)
{
	return vertices_[index];
}

const Vertex& Triangle::GetVertex(int index) const
{
	return vertices_[index];
}
