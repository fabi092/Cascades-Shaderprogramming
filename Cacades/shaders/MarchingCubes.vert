#version 330 core
layout(location = 0) in vec2 position;

struct Noise
{
	mat4 rotation;
	sampler3D tex;
};

uniform int layerStart;
uniform int layerCorrection;
uniform vec3 resolution;
uniform int isoLevel = 0;
uniform sampler3D densityTex;
uniform Noise noise[4];
uniform float noiseScale = 1;

out Gridcell {
   vec3 p[8];
   float val[8];
   int mc_case;
} vs_out;

vec3 ws_to_UVW(vec3 ws)
{
	vec3 scaled = ws * 0.5f + 0.5f;
	return vec3(scaled.xz, scaled.y);
}

float GetNoise(vec3 texCoord)
{
	float value = 0;
	for (int i = 0; i < 4; ++i)
	{
		value += texture(noise[i].tex, (noise[i].rotation * vec4(texCoord.zxy, 1.0f)).xyz).r;
	}
	return value;
}

void main()
{
	float layer = -1 + (resolution.y * (gl_InstanceID + layerStart + 0.5f));
	vec3 ws = vec3(position.x, layer, position.y);
	float noiseCorrection = -resolution.y * layerCorrection;

	
	vs_out.p[0] = ws + (resolution * vec3(0, 0, 0));
	vs_out.p[1] = ws + (resolution * vec3(1, 0, 0));
	vs_out.p[2] = ws + (resolution * vec3(1, 0, 1));
	vs_out.p[3] = ws + (resolution * vec3(0, 0, 1));
	vs_out.p[4] = ws + (resolution * vec3(0, 1, 0));
	vs_out.p[5] = ws + (resolution * vec3(1, 1, 0));
	vs_out.p[6] = ws + (resolution * vec3(1, 1, 1));
	vs_out.p[7] = ws + (resolution * vec3(0, 1, 1));


	for (int i = 0; i < 8; ++i)
	{
		vec3 texCoord = ws_to_UVW(vs_out.p[i]);
		vec3 noiseCoord = ws_to_UVW(vs_out.p[i] - vec3(0, noiseCorrection, 0));
		vs_out.val[i] = texture(densityTex, texCoord).r + noiseScale * GetNoise(noiseCoord * 4.0f);
	}

	/*
      Determine the index into the edge table which
      tells us which vertices are inside of the surface
    */
    int cubeindex = 0;
    if (vs_out.val[0] <= isoLevel) cubeindex |= 1;
    if (vs_out.val[1] <= isoLevel) cubeindex |= 2;
    if (vs_out.val[2] <= isoLevel) cubeindex |= 4;
    if (vs_out.val[3] <= isoLevel) cubeindex |= 8;
    if (vs_out.val[4] <= isoLevel) cubeindex |= 16;
    if (vs_out.val[5] <= isoLevel) cubeindex |= 32;
    if (vs_out.val[6] <= isoLevel) cubeindex |= 64;
    if (vs_out.val[7] <= isoLevel) cubeindex |= 128;

	vs_out.mc_case = cubeindex;
}
