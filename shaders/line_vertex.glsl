#version 330 core
layout(location = 0) in vec2 aPos;

uniform vec2 screenSize;

void main()
{
	vec2 normalizedPos = (aPos / screenSize) * 2.0 - 1.0;
	normalizedPos.y = -normalizedPos.y;
	
	gl_Position = vec4(normalizedPos, 0.0, 1.0);
}
