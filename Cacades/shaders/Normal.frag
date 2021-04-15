#version 430 core

layout(location = 0) out vec3 normal;

in GS_OUT
{
	vec3 ws;
} fs_in;

uniform vec3 resolution;
uniform sampler3D densityTex;

vec3 ws_to_UVW(vec3 ws)
{
	vec3 scaled = ws * 0.5f + 0.5f;
	return vec3(scaled.xz, scaled.y);
}

vec3 ComputeNormal(vec3 ws)
{
	vec3 gradient = vec3(
		  texture(densityTex, ws_to_UVW(ws + vec3(resolution.x, 0, 0))).r
     	- texture(densityTex, ws_to_UVW(ws - vec3(resolution.x, 0, 0))).r,
		  texture(densityTex, ws_to_UVW(ws + vec3(0, resolution.y, 0))).r
        - texture(densityTex, ws_to_UVW(ws - vec3(0, resolution.y, 0))).r,
		  texture(densityTex, ws_to_UVW(ws + vec3(0, 0, resolution.z))).r
        - texture(densityTex, ws_to_UVW(ws - vec3(0, 0, resolution.z))).r	);
	return normalize(-gradient);
}

void main()
{
	normal = ComputeNormal(fs_in.ws);
}
