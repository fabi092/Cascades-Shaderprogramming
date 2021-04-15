#pragma once
#include <glm/detail/type_vec2.hpp>
#include "Triangle.h"

class Plane 
{
public:
	static Triangle* GetTris(glm::vec2 size = glm::vec2(1), glm::vec2 textureRepeat = glm::vec2(1));
};

