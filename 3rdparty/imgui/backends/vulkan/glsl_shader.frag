#version 450 core
layout(location = 0) out vec4 fColor;

layout(set=0, binding=0) uniform sampler2D sTexture;

layout(location = 0) in struct {
    vec4 Color;
    vec2 UV;
} In;

// https://github.com/ocornut/imgui/issues/4890
vec4 to_linear(vec4 sRGB)
{
	bvec3 cutoff = lessThan(sRGB.rgb, vec3(0.04045));
	vec3  higher = pow((sRGB.rgb + vec3(0.055)) / vec3(1.055), vec3(2.4));
	vec3  lower  = sRGB.rgb / vec3(12.92);

	return vec4(mix(higher, lower, cutoff), sRGB.a);
}

void main()
{
	fColor = to_linear(In.Color) * to_linear(texture(sTexture, In.UV.st));
}

/*
void main()
{
    fColor = In.Color * texture(sTexture, In.UV.st);
	fColor.rgb = pow(fColor.rgb, vec3(2.2));
}
*/
