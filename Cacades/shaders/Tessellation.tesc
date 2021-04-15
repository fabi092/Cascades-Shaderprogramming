#version 440 core

layout (vertices=3) out;

uniform int MinTessLevel = 1;
uniform int MaxTessLevel = 32;
uniform float MinDepth = 1;
uniform float MaxDepth = 16;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	vec4 p = projection * view * model * gl_in[gl_InvocationID].gl_Position;
	float depth = clamp( (abs(p.z) - MinDepth) / (MaxDepth - MinDepth), 0.0f, 1.0f);
	float TessLevel = mix(MaxTessLevel, MinTessLevel, depth);

	gl_TessLevelInner[0] = float(TessLevel);
    gl_TessLevelOuter[0] = float(TessLevel);
    gl_TessLevelOuter[1] = float(TessLevel);
    gl_TessLevelOuter[2] = float(TessLevel);
}
