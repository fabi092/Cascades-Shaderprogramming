#version 430 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT
{
	int layer;
	vec3 ws;
} gs_in[];

out GS_OUT
{
	vec3 ws;
} gs_out;

void main()
{
	for (int i = 0; i < 3; ++i)
	{
		gl_Layer = gs_in[i].layer;
		gs_out.ws = gs_in[i].ws;
		gl_Position = vec4(gs_in[i].ws.xz, 0.0, 1.0);
		EmitVertex();
	}
	EndPrimitive();
}