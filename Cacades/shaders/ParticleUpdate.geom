#version 330 core
layout (points) in;
layout (points, max_vertices = 5) out;

in Particle
{
	vec3 position;
	vec3 velocity;
	vec2 seed;
	flat float lifeTime;
	flat int type;
} gs_in[];

struct ParticleOut
{
	vec3 position;
	vec3 velocity;
	vec2 seed;
	float lifeTime;
	int type;
};
out ParticleOut gs_out;

uniform sampler3D densityTex;
uniform sampler3D normalTex;
uniform float waterTTL = 1.0f;
uniform float mistTTL = 2.0f;
uniform float deltaTime;
uniform float isoLevel = 0;
uniform float particlesPerSecond = 2;
uniform float velocityScale = 0.01f;
uniform float maxRayLenght = 10;
uniform vec3 resolution;

vec2 randomSeed;

#pragma include "EnumParticleType.glh"

void InitRandom()
{
	randomSeed = gs_in[0].seed;
}

float Random()
{
	float value = fract(sin(dot(randomSeed, vec2(12.9898,78.233))) * 43758.5453);
	randomSeed.x += value;
	randomSeed.y -= value;
	return value;
}

float Random(float min, float max)
{
	float value = Random();
	value *= (max - min) / 2;
	value += (max - min) / 2 + min;
	return value;
}

mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

vec3 ws_to_UVW(vec3 ws)
{
    vec3 scaled = ws * 0.5f + 0.5f;
    return vec3(scaled.xz, scaled.y);
}

void SpawnEmitters()
{
    vec3 origin = gs_in[0].position;
	int steps = 1000;
	vec3 dir = maxRayLenght / steps * gs_in[0].velocity;

   for (int i = 0; i < steps; ++i)
	{
       origin += dir;
       if (texture(densityTex, ws_to_UVW(origin)).r > isoLevel)
		{
           gs_out.position = origin - dir;
           gs_out.velocity = texture(normalTex, ws_to_UVW(origin)).rgb;
           gs_out.lifeTime = 0;
           gs_out.seed = vec2(Random(), Random());
           gs_out.type = PARTICLE_EMITTER;
           EmitVertex();
           EndPrimitive();
           return;
       }
	}
}

void SpawnWater()
{
    gs_out.position = gs_in[0].position;
    gs_out.velocity = gs_in[0].velocity;
    gs_out.lifeTime = gs_in[0].lifeTime + deltaTime;
    gs_out.seed = vec2(Random(), Random());
    gs_out.type = gs_in[0].type;

    float spawnRate = 1 / particlesPerSecond;
    bool spawn = gs_out.lifeTime >= spawnRate;

    if (spawn)
		gs_out.lifeTime -= spawnRate;

    EmitVertex();
    EndPrimitive();

    if (spawn)
	{
		gs_out.position = gs_in[0].position + gs_in[0].velocity * velocityScale;
		gs_out.velocity = gs_in[0].velocity * 10.0f * velocityScale;
		gs_out.lifeTime = 0;
		gs_out.seed = vec2(Random(), Random());
		gs_out.type = PARTICLE_WATER_FALLING;
		EmitVertex();
		EndPrimitive();
    }
}

void UpdateWater()
{
	gs_out.lifeTime = gs_in[0].lifeTime + deltaTime;

	if (gs_out.lifeTime >= waterTTL)
		return;

	gs_out.velocity = gs_in[0].velocity - vec3(0, 9.81f, 0) * deltaTime * velocityScale;
	gs_out.position = gs_in[0].position + gs_out.velocity * deltaTime;

	if (texture(densityTex, ws_to_UVW(gs_out.position)).r > isoLevel)
	{
		{
			vec3 normal = texture(normalTex, ws_to_UVW(gs_out.position)).rgb;

			gs_out.position = gs_out.position - gs_out.velocity * deltaTime;
			gs_out.velocity = vec3(0);
			gs_out.seed = vec2(Random(), Random());
			gs_out.type = PARTICLE_MIST_COLLISION;
			EmitVertex();
			EndPrimitive();
			return;
		}
	}

	gs_out.seed = vec2(Random(), Random());
	gs_out.type = gs_in[0].type;
	EmitVertex();
	EndPrimitive();
}

void UpdateMist()
{
	gs_out.lifeTime = gs_in[0].lifeTime + deltaTime;

	if (gs_out.lifeTime >= waterTTL)
		return;

	gs_out.velocity = gs_in[0].velocity + vec3(0, 9.81f, 0) * deltaTime * velocityScale / 5.0f;
	gs_out.position = gs_in[0].position + gs_out.velocity * deltaTime;

	if (texture(densityTex, ws_to_UVW(gs_out.position)).r > isoLevel)
	{
		gs_out.position = gs_out.position - gs_out.velocity * deltaTime;
	}

	gs_out.seed = vec2(Random(), Random());
	gs_out.type = gs_in[0].type;
	EmitVertex();
	EndPrimitive();
}

void main()
{
	InitRandom();

    switch(gs_in[0].type)
	{
        case PARTICLE_RAY: SpawnEmitters();
        break;
        case PARTICLE_EMITTER: SpawnWater();
        break;
        case PARTICLE_WATER_FLOWING:
		case PARTICLE_WATER_FALLING: UpdateWater();
        break;
        case PARTICLE_MIST_COLLISION:
		case PARTICLE_MIST_FALLING: UpdateMist();
        break;
    }
}
