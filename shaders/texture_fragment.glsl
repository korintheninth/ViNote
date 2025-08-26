#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D textureSampler;
uniform vec3 tintColor;
uniform float intensity;

void main()
{
	vec4 tex = texture(textureSampler, TexCoord);
	vec3 blended = mix(tex.rgb, tintColor.rgb, intensity);
	FragColor = vec4(blended, tex.a);
}
