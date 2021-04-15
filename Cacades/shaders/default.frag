#version 330 core

out vec4 FragColor;

#line 1 1 
#pragma include "EnumLightType.glh"
#line 1 2
#pragma include "EnumShadowMode.glh"
#line 1 3
#pragma include "EnumColorMode.glh"
#line 1 4
#pragma include "EnumNormalMode.glh"
#line 1 5
#pragma include "EnumDisplacementMode.glh"
#line 1 6
#pragma include "Lighting.glh"
#line 17 0

in VS_OUT
{
	vec3 FragPos;
	vec3 Normal;
	vec2 UV;
	mat3 TBN;
} fs_in;

const int LIGHT_COUNT = 4;

uniform LightSource Lights[LIGHT_COUNT];

uniform int ShadowType = HARD_SHADOWS;
uniform bool EnableLighting = true;

uniform int colorMode;
uniform vec3 objectColor;
uniform sampler2D objectTexture;

uniform int normalMode;
uniform sampler2D normalMap;
uniform float bumbiness = 2.0f;

uniform int displacementMode;
uniform sampler2D displacementMap;
//uniform int displacement_initialSteps;
uniform int displacement_refinementSteps;
uniform float heightScale = 0.025f;

uniform vec3 viewPos;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{ 
    // number of depth layers
    const float minLayers = 16;
    const float maxLayers = 64;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
    
	// calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = vec2(viewDir.x, -viewDir.y) / viewDir.z * heightScale; 
    vec2 deltaTexCoords = P / numLayers;
  
    // get initial values
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = texture(displacementMap, currentTexCoords).r;

 	while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(displacementMap, currentTexCoords).r;  
        // get depth of next layer
        currentLayerDepth += layerDepth;  
    }
	
    currentTexCoords += deltaTexCoords;
    currentDepthMapValue = texture(displacementMap, currentTexCoords).r;  
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
        currentDepthMapValue = texture(displacementMap, currentTexCoords).r;  
        // get depth of next layer
        currentLayerDepth += layerDepth;  
    }
    
    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
	float prevLayerDepth = currentLayerDepth - layerDepth; 

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(displacementMap, prevTexCoords).r - prevLayerDepth;
 
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
} 

vec2 DetermineTextureCoords(vec2 uv)
{
	switch(displacementMode)
	{	
		case DISPLACEMENT_HEIGHT_MODE:
			return vec2(0,0);
		case DISPLACEMENT_DEPTH_MODE:
		{
			mat3 AntiTBN = transpose(fs_in.TBN);
			vec3 tangentViewDir = normalize((AntiTBN * viewPos) - (AntiTBN * fs_in.FragPos));
			vec2 texCoords = ParallaxMapping(fs_in.UV,  tangentViewDir);

			return texCoords;
		}
		case DISPLACEMENT_NON_MODE:
			return uv;
	}
}

vec3 DetermineFragmentColor(in int colorMode, vec2 uv)
{
	//Check for blending colorMode
	switch (colorMode)
	{
		case COLOR_ONLY_MODE:
			return objectColor;
		case COLOR_MIX_MODE:
			return texture(objectTexture, uv).rgb * objectColor;
		case TEXTURE_ONLY_MODE:
			return texture(objectTexture, uv).rgb;
	}
}

vec3 DetermineFragmentNormal(in int normalMode, vec2 uv)
{
	switch (normalMode)
	{
		case NORMALS_ONLY_MODE:
			return fs_in.Normal;
		case NO_NORMALS_MODE:
			return vec3(0.0f);
		case BUMP_MAP_ONLY_MODE:
			// Obtain normal from normal map in range [0,1]
			vec3 tmpNormal = texture(normalMap, uv).rgb;
			// Transform normal vector to range [-1,1]
			tmpNormal = normalize(tmpNormal * 2.0 - 1.0);
			// Apply "Bumpiness"
			tmpNormal = normalize(tmpNormal * vec3(bumbiness, -bumbiness, 1.0f));
			// Convert to global
			return normalize(fs_in.TBN * tmpNormal);
	}
}

void main()
{	
	vec2 texCoords = DetermineTextureCoords(fs_in.UV);

	vec3 color = DetermineFragmentColor(colorMode, texCoords);

	vec3 normal = DetermineFragmentNormal(normalMode, texCoords);

	if (!EnableLighting || normal == vec3(0.0f))
	{
		FragColor = vec4(color, 1.0f);
		return;
	}	

	LightingGlobals globals = LightingGlobals(viewPos, fs_in.FragPos, normal, EnableLighting);

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