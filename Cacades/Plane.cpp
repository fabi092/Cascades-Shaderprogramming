#include "Plane.h"

Triangle* Plane::GetTris(glm::vec2 size, glm::vec2 textureRepeat)
{
	Triangle* tris = new Triangle[2]
	{
		Triangle(Vertex(glm::vec3(+0.5f * size.x, 0, -0.5f * size.y), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f * textureRepeat.x, 0.0f * textureRepeat.y)),
				 Vertex(glm::vec3(-0.5f * size.x, 0, -0.5f * size.y), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f * textureRepeat.x, 0.0f * textureRepeat.y)),
				 Vertex(glm::vec3(-0.5f * size.x, 0, +0.5f * size.y), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f * textureRepeat.x, 1.0f * textureRepeat.y))),
		Triangle(Vertex(glm::vec3(-0.5f * size.x, 0, +0.5f * size.y), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f * textureRepeat.x, 1.0f * textureRepeat.y)),
				 Vertex(glm::vec3(+0.5f * size.x, 0, +0.5f * size.y), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f * textureRepeat.x, 1.0f * textureRepeat.y)),
				 Vertex(glm::vec3(+0.5f * size.x, 0, -0.5f * size.y), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f * textureRepeat.x, 0.0f * textureRepeat.y)))
	};
	return tris;
}
