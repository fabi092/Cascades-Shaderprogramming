#version 440 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec3 UVW;
    mat3 TBN;
} gs_in[];

out VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec3 UVW;
    mat3 TBN;
} gs_out;

void main()
{
    for (int i = 0; i < 3; ++i)
    {
    	gs_out.FragPos = gs_in[i].FragPos;
    	gs_out.Normal = gs_in[i].Normal;
    	gs_out.UVW = gs_in[i].UVW;
    	gs_out.TBN = gs_in[i].TBN;
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }

    EndPrimitive();
}
