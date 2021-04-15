#pragma once

#include "shaders/EnumNormalMode.glh"
#include "shaders/EnumColorMode.glh"
#include "shaders/EnumLightType.glh"
#include "shaders/EnumParticleType.glh"
#include "shaders/EnumShadowMode.glh"
#include "shaders/EnumDisplacementMode.glh"

enum DisplacementMode
{
	DepthDisplacement = DISPLACEMENT_DEPTH_MODE,
	HeightDisplacement = DISPLACEMENT_HEIGHT_MODE,
	NoDisplacement = DISPLACEMENT_NON_MODE,
};

enum NormalBlendMode
{
	NormalsOnly = NORMALS_ONLY_MODE,
	NoNormals = NO_NORMALS_MODE,
	BumpMapOnly = BUMP_MAP_ONLY_MODE,
};

enum ColorBlendMode
{
	ColorOnly = COLOR_ONLY_MODE,
	Mix = COLOR_MIX_MODE,
	TextureOnly = TEXTURE_ONLY_MODE,
};

enum ParticleType
{
	EmitterRay = PARTICLE_RAY,
	Emitter = PARTICLE_EMITTER,
	FlowingWater = PARTICLE_WATER_FLOWING,
	FallingWater = PARTICLE_WATER_FALLING,
	CollisionMist = PARTICLE_MIST_COLLISION,
	FallingMist = PARTICLE_MIST_FALLING,
};

enum State
{
	Loading,
	Running,
	Paused,
	Stopped,
};

enum LightType
{
	Directional = DIR_LIGHT,
	Spot = SPOT_LIGHT,
	Point = POINT_LIGHT,
};

enum ShadowMode
{
	HardShadows = HARD_SHADOWS,
	PcfShadows = PCF_SHADOWS,
	VsmShadows = VSM_SHADOWS,
};