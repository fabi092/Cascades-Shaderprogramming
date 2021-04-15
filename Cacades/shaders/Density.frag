#version 430 core
#define M_PI 3.1415926535897932384626433832795

layout(location = 0) out float density;

in GS_OUT
{
	vec3 ws;
} fs_in;

struct Randoms
{
	float offset;
	int frequenceSign;
	float frequence;
};

uniform Randoms pillars[4];
uniform Randoms helix;
uniform Randoms shelf;

vec2 rotate(vec2 v, float a)
{
	float s = sin(a);
	float c = cos(a);
	mat2 m = mat2(c, -s, s, c);
	return m * v;
}

float AddPillar(vec3 ws, vec2 position, Randoms randoms, float weight)
{
	float frequence = randoms.frequenceSign * (1.0f + mod(randoms.frequence, 4.0f));
	float angle = /*randoms.offset +*/ frequence * ws.y * M_PI;

	position = rotate(position, angle);
	return weight * (1.0f / length(ws.xz - position) - 1.0f);
}

float AddBounds(vec3 ws, float weight)
{
	return weight * pow(length(ws.xz), 3);
}

float AddHelix(vec3 ws, Randoms randoms, float weight)
{
	float frequence = randoms.frequenceSign * (8.0f + mod(randoms.frequence, 5.0f));
	float angle = randoms.offset + frequence * ws.y * M_PI;

	float sinLayer = sin(angle);
	float cosLayer = cos(angle);
	return weight * dot(vec2(cosLayer, sinLayer), ws.xz);
}

float AddShelf(vec3 ws, Randoms randoms, float weight)
{
	float frequence = randoms.frequenceSign * (8.0f + mod(randoms.frequence, 5.0f));
	float angle = randoms.offset + frequence * ws.y * M_PI;

	float cosLayer = cos(angle);
	return weight * cosLayer;
}

void main()
{
	density = 0;
	density += AddPillar(fs_in.ws, vec2( 0.0f,  0.50f), pillars[0], 0.25f);
	density += AddPillar(fs_in.ws, vec2(-0.4f, -0.25f), pillars[1], 0.25f);
	density += AddPillar(fs_in.ws, vec2( 0.4f, -0.25f), pillars[2], 0.25f);
	density += AddPillar(fs_in.ws, vec2( 0.0f,  0.00f), pillars[3], -1.0f);
	density += AddBounds(fs_in.ws, -10);
	density += AddHelix(fs_in.ws, helix, 3.0f);
	density += AddShelf(fs_in.ws, shelf, 2.0f);
}