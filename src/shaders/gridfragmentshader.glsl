#version 330 core
in vec2 fragPos;
out vec4 FragColor;

uniform float gridSpacing ;
uniform float lineWidth;
uniform vec3 gridColor = vec3(0.0, 0.0, 0.0);

void main()
{
	vec2 uv = fragPos;

	vec2 grid = abs(fract(uv / gridSpacing - 0.5) - 0.5) / fwidth(uv / gridSpacing);
	float line = min(grid.x, grid.y);

	if (line > lineWidth) {
		discard;
	}

	FragColor = vec4(gridColor, 1.0);
}
