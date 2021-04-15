#version 330 core

out vec2 varianceShadowMap;

void main()
{
	float lightDistance = gl_FragCoord.z;
	gl_FragDepth = lightDistance;
	varianceShadowMap = vec2(lightDistance, lightDistance * lightDistance);
}