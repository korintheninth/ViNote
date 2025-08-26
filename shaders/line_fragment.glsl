#version 330 core
out vec4 FragColor;

uniform vec3 lineColor = vec3(1.0, 0.0, 0.0);
uniform bool erase = false;

void main()
{
	if (erase) {
		FragColor = vec4(0.0, 0.0, 0.0, 0.0);
		return;
	}
	FragColor = vec4(lineColor, 1.0);
}
