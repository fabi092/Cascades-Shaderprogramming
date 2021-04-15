#version 440 core

layout (vertices=3) out;

in VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec3 UVW;
    mat3 TBN;
} tc_in[];

out VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec3 UVW;
    mat3 TBN;
} tc_out[];

out PhongPatch
{
	 float termIJ;
	 float termJK;
	 float termIK;
} tc_PhongPatch[];

uniform int MinTessLevel = 1;
uniform int MaxTessLevel = 8;
uniform float MinDepth = 1;
uniform float MaxDepth = 4;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

float PIi(int i, vec3 q)
{
 vec3 q_minus_p = q - gl_in[i].gl_Position.xyz;
 return q[gl_InvocationID] - dot(q_minus_p, tc_in[i].Normal) * tc_in[i].Normal[gl_InvocationID];
}

#define Pi gl_in[0].gl_Position.xyz
#define Pj gl_in[1].gl_Position.xyz
#define Pk gl_in[2].gl_Position.xyz

void main()
{
	//Get Data
	tc_out[gl_InvocationID].FragPos = tc_in[gl_InvocationID].FragPos;
	tc_out[gl_InvocationID].Normal = tc_in[gl_InvocationID].Normal;
	tc_out[gl_InvocationID].UVW = tc_in[gl_InvocationID].UVW;
	tc_out[gl_InvocationID].TBN = tc_in[gl_InvocationID].TBN;
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

	// compute patch data
	 tc_PhongPatch[gl_InvocationID].termIJ = PIi(0,Pj) + PIi(1,Pi);
	 tc_PhongPatch[gl_InvocationID].termJK = PIi(1,Pk) + PIi(2,Pj);
	 tc_PhongPatch[gl_InvocationID].termIK = PIi(2,Pi) + PIi(0,Pk);

	//Calc TessLevel
	vec4 p = projection * view * gl_in[gl_InvocationID].gl_Position;
	float depth = clamp( (abs(p.z) - MinDepth) / (MaxDepth - MinDepth), 0.0f, 1.0f);
	float TessLevel = mix(MaxTessLevel, MinTessLevel, depth);

	gl_TessLevelInner[0] = float(TessLevel);
    gl_TessLevelOuter[0] = float(TessLevel);
    gl_TessLevelOuter[1] = float(TessLevel);
    gl_TessLevelOuter[2] = float(TessLevel);
}
