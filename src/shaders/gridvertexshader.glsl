#version 330 core
layout(location = 0) in vec4 aPos;

uniform vec2 screenSize;
out vec2 fragPos;

void main()
{
	vec2 normalizedPos = (aPos.xy / screenSize) * 2.0 - 1.0;
	normalizedPos.y = -normalizedPos.y;
	
	fragPos = aPos.xy;
	gl_Position = vec4(normalizedPos, 0.0, 1.0);
}
