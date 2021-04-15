#version 440 core

layout(triangles, fractional_odd_spacing, ccw) in;

in VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec3 UVW;
    mat3 TBN;
} te_in[];

in PhongPatch
{
	 float termIJ;
	 float termJK;
	 float termIK;
} te_PhongPatch[];

out VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec3 UVW;
    mat3 TBN;
} te_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

#define Pi  gl_in[0].gl_Position.xyz
#define Pj  gl_in[1].gl_Position.xyz
#define Pk  gl_in[2].gl_Position.xyz
#define tc1 gl_TessCoord

uniform float TessAlpha = 1;

void main()
{
	// precompute squared tesscoords
	 vec3 tc2 = tc1*tc1;

	 // compute texcoord and normal
	 te_out.UVW = gl_TessCoord[0]*te_in[0].UVW
	           	+ gl_TessCoord[1]*te_in[1].UVW
	           	+ gl_TessCoord[2]*te_in[2].UVW;
	 te_out.Normal = gl_TessCoord[0]*te_in[0].Normal
	           	   + gl_TessCoord[1]*te_in[1].Normal
	               + gl_TessCoord[2]*te_in[2].Normal;

	 // interpolated position
	 vec3 barPos = gl_TessCoord[0]*Pi
	             + gl_TessCoord[1]*Pj
	             + gl_TessCoord[2]*Pk;

	 // build terms
	 vec3 termIJ = vec3(te_PhongPatch[0].termIJ,
	                    te_PhongPatch[1].termIJ,
	                    te_PhongPatch[2].termIJ);
	 vec3 termJK = vec3(te_PhongPatch[0].termJK,
	                    te_PhongPatch[1].termJK,
	                    te_PhongPatch[2].termJK);
	 vec3 termIK = vec3(te_PhongPatch[0].termIK,
	                    te_PhongPatch[1].termIK,
	                    te_PhongPatch[2].termIK);

	 // phong tesselated pos
	 vec3 phongPos   = tc2[0]*Pi
	                 + tc2[1]*Pj
	                 + tc2[2]*Pk
	                 + tc1[0]*tc1[1]*termIJ
	                 + tc1[1]*tc1[2]*termJK
	                 + tc1[2]*tc1[0]*termIK;

	 // final position
	 te_out.FragPos = (1.0-TessAlpha)*barPos + TessAlpha*phongPos;
	 gl_Position   = projection * view * vec4(te_out.FragPos,1.0);
}
