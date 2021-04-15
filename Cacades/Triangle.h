#pragma once
#include <glm/gtx/norm.hpp>
#include "Vertex.h"

struct Triangle
{
	Triangle(Vertex v0, Vertex v1, Vertex v2);
	Triangle(Vertex vertices[3]);

	Vertex& GetVertex(int index);
	const Vertex& GetVertex(int index) const;

protected:
	Vertex vertices_[3];
};
