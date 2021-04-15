#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 uvw;

out VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec3 UVW;
    mat3 TBN;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position	= model * vec4(position, 1.0f);
    vs_out.FragPos = vec3(model * vec4(position, 1.0f));
    vs_out.Normal = normalize(vec3(transpose(inverse(model)) * vec4(normal, 1.0f)));
    vs_out.UVW = uvw;

    vec3 tangent = vec3(1, 0, 0);
    vec3 binormal = normalize(cross(tangent, vs_out.Normal));
    tangent = normalize(cross(vs_out.Normal, binormal));
    mat3 TBN = transpose(mat3(tangent, binormal, vs_out.Normal));

  	vs_out.TBN = TBN;
}