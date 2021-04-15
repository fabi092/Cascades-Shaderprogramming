#version 330 core

out vec4 FragColor;

#pragma include "EnumShadowMode.glh"
#pragma include "Lighting.glh"

in VS_OUT
{
	vec3 FragPos;
	vec3 Normal;
	vec3 UVW;
	mat3 TBN;
} fs_in;

const int LIGHT_COUNT = 1;

uniform LightSource Lights[LIGHT_COUNT];

uniform int ShadowType = HARD_SHADOWS;
uniform bool EnableLighting = true;

uniform sampler2D objectTexture[3];

uniform sampler2D normalMap[3];
uniform float bumbiness = 1f;

uniform sampler2D displacementMap[3];
uniform int displacement_initialSteps = 8;
uniform int displacement_refinementSteps = 8;
uniform float displacement_scale = 0.025f;

uniform vec3 viewPos;

vec2 Parallax(sampler2D map, vec2 texCoords, vec3 viewDir)
{
    return texCoords;

    // calculate the size of each layer
    float layerDepth = 1.0 / displacement_initialSteps;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = vec2(viewDir.x, -viewDir.y) / viewDir.z * displacement_scale;
    vec2 deltaTexCoords = P / displacement_initialSteps;

    // get initial values
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = texture(map, currentTexCoords).r;

 	while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(map, currentTexCoords).r;
        // get depth of next layer
        currentLayerDepth += layerDepth;
    }

    currentTexCoords += deltaTexCoords;
    currentDepthMapValue = texture(map, currentTexCoords).r;
    currentLayerDepth -= layerDepth;
	currentLayerDepth -= 0.085f; //reduces artifacts

	// decrease the step size as we do the refinement steps
	deltaTexCoords /= displacement_refinementSteps;
	layerDepth /= displacement_refinementSteps;

 	while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(map, currentTexCoords).r;
        // get depth of next layer
        currentLayerDepth += layerDepth;
    }

    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
	float prevLayerDepth = currentLayerDepth - layerDepth;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(map, prevTexCoords).r - prevLayerDepth;

    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}

vec3 NormalizeNormal(vec3 tmpNormal)
{
  // Transform normal vector to range [-1,1]
  tmpNormal = normalize(tmpNormal * 2.0 - 1.0);
  // Apply "Bumpiness"
  return normalize(tmpNormal * vec3(bumbiness, bumbiness, 1.0f));
}

void main()
{
    vec3 blending = abs( fs_in.Normal );
    blending = normalize(max(blending, 0.00001)); // Force weights to sum to 1.0
    float b = (blending.x + blending.y + blending.z);
    blending /= vec3(b, b, b);

	mat3 AntiTBN = transpose(fs_in.TBN);
	vec3 tangentViewDir = normalize((AntiTBN * viewPos) - (AntiTBN * fs_in.FragPos));

    vec2[3] uvs;
    uvs[0] = fs_in.UVW.yz;
    uvs[1] = fs_in.UVW.xz;
    uvs[2] = fs_in.UVW.xy;

    vec3[3] colors;
    vec3[3] normals;
    for (int i = 0; i < 3; ++i)
    {
        uvs[i] = Parallax(displacementMap[i], uvs[i], tangentViewDir);
        colors[i] = texture(objectTexture[i], uvs[i]).xyz;
        normals[i] = NormalizeNormal(texture(normalMap[i], uvs[i]).xyz);
    }

    vec3 color = colors[0] * blending[0] + colors[1] * blending[1] + colors[2] * blending[2];

    vec3 normal = fs_in.Normal;//normals[0] * blending[0] + normals[1] * blending[1] + normals[2] * blending[2];
    //normal = transpose(fs_in.TBN) * normal;

	if (!EnableLighting || normal == vec3(0.0f))
	{
		FragColor = vec4(color, 1.0f);
		return;
	}

	LightingGlobals globals = LightingGlobals(viewPos, fs_in.FragPos, fs_in.Normal, EnableLighting);

	vec3 lighting = vec3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < LIGHT_COUNT; i++)
	{
		if (!Lights[i].IsEnabled)
			continue;

			lighting += CalculateDirLightSource(Lights[i], globals);

	}

	lighting = clamp(lighting, 0, 1);
	lighting *= color;

	FragColor = vec4(lighting, 1.0f);
}
