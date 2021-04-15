#pragma once
#include "Model.h"

class Box 
{
public:
	static Triangle* GetTris(glm::vec3 size = glm::vec3(1), glm::vec3 textureRepeat = glm::vec3(1));
};

