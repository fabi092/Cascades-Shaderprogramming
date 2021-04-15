#pragma once
#include <glm/detail/type_vec3.hpp>
#include "Model.h"

struct RenderInfo
{
	float NormalMapFactor = 1.0f;
	ShadowMode ShadowMode = HardShadows;
	bool EnableLight = true;
	bool EnableShadows = true;
	bool DrawLightPosition = true;
	bool RenderPath = false;

	//Generator
	glm::ivec3 Resolution;
	int Seed = 0;
	int StartLayer = 0;
	float NoiseScale = 1.0f;;
	glm::vec3 GeometryScale;
	float IsoLevel = 1.0f;;
	bool WireFrameMode = 0;

	int DisplacementInitialSteps = 16;
	int DisplacementRefinementSteps = 16;
	float heightScale = 0.025f;
};
