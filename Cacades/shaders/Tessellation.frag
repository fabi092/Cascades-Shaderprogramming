#version 440 core

in vec3 gPosition;
in vec3 gFacetNormal;
in vec3 gTriDistance;
in vec3 gPatchDistance;

out vec4 FragColor;


#pragma include "EnumShadowMode.glh"
#pragma include "Lighting.glh"

const int LIGHT_COUNT = 1;
uniform LightSource Lights[LIGHT_COUNT];
uniform int ShadowType = HARD_SHADOWS;
uniform bool EnableLighting = true;

uniform vec3 objectColor = vec3(1);
uniform vec3 viewPos;

float amplify(float d, float scale, float offset)
{
    d = scale * d + offset;
    d = clamp(d, 0, 1);
    d = 1 - exp2(-2*d*d);
    return d;
}

void main()
{
    vec3 lighting;

    if (EnableLighting)
    {
        LightingGlobals globals = LightingGlobals(viewPos, gPosition, gFacetNormal, EnableLighting);

    	lighting = vec3(0.0f, 0.0f, 0.0f);
    	for (int i = 0; i < LIGHT_COUNT; i++)
    	{
    		if (!Lights[i].IsEnabled)
    			continue;
    			lighting += CalculateDirLightSource(Lights[i], globals);
    		
    	}
        lighting = clamp(lighting, 0, 1);
        lighting *= objectColor;
    }
    else
    {
        lighting = objectColor;
    }

	float d1 = min(min(gTriDistance.x, gTriDistance.y), gTriDistance.z);
	float d2 = min(min(gPatchDistance.x, gPatchDistance.y), gPatchDistance.z);
	lighting = amplify(d1, 40, -0.5) * amplify(d2, 60, -0.5) * lighting;
	FragColor = vec4(lighting, 1.0f);
}