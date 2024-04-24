#version 450

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec3 fragPositionWS;
layout (location = 2) in vec3 fragNormalWS;

layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 0) uniform GlobalUbo
{
	mat4 projectionMatrix;
	mat4 viewMatrix;
	vec4 ambientLightColor;
	vec3 lightPosition;
	vec4 lightColor;
} ubo;

layout (push_constant) uniform Push
{
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

void main()
{
	vec3 lightDir = ubo.lightPosition - fragPositionWS.xyz;
	float attenuation = 1.0 / dot(lightDir, lightDir);
	lightDir = normalize(lightDir);

	vec3 lightColor = ubo.lightColor.xyz * ubo.lightColor.w * attenuation;
	vec3 ambientLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
	vec3 diffuseLight = lightColor * max(dot(normalize(fragNormalWS), lightDir), 0);

	outColor = vec4((diffuseLight + ambientLight) * fragColor, 1.0);
}
