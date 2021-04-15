#version 430 core
layout(location = 0) in vec2 position;

out VS_OUT
{
	int layer;
	vec3 ws;
} vs_out;

uniform vec3 resolution;

void main()
{
	vs_out.layer = gl_InstanceID;
	vs_out.ws = vec3(position.x, 2.0f * (gl_InstanceID / resolution.y - 0.5f), position.y);
}
