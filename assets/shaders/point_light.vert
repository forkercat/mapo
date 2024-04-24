#version 450

const vec2 OFFSETS[6] = vec2[](
	vec2(-1.0, -1.0),
	vec2(-1.0, 1.0),
	vec2(1.0, -1.0),
	vec2(1.0, -1.0),
	vec2(-1.0, 1.0),
	vec2(1.0, 1.0)
);

layout (location = 0) out vec2 fragOffset;

layout (set = 0, binding = 0) uniform GlobalUbo
{
	mat4 projectionMatrix;
	mat4 viewMatrix;
	vec4 ambientLightColor;
	vec3 lightPosition;
	vec4 lightColor;
} ubo;

const float LIGHT_RADIUS = 0.05;

void main()
{
	fragOffset = OFFSETS[gl_VertexIndex];
	vec3 cameraRightWS = { ubo.viewMatrix[0][0], ubo.viewMatrix[1][0], ubo.viewMatrix[2][0] };
	vec3 cameraUpWS = { ubo.viewMatrix[0][1], ubo.viewMatrix[1][1], ubo.viewMatrix[2][1] };

	vec3 positionWS = ubo.lightPosition.xyz
		+ LIGHT_RADIUS * fragOffset.x * cameraRightWS
		+ LIGHT_RADIUS * fragOffset.y * cameraUpWS;

	gl_Position = ubo.projectionMatrix * ubo.viewMatrix * vec4(positionWS, 1.0);
}
