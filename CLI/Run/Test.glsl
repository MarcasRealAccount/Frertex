#version 460 core

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec4 outColor;

void funcB(int depth);

void funcA(int depth)
{
	funcB(depth + 1);
}

void funcB(int depth)
{
	if (depth < 32)
		funcA(depth + 1);
}

void main()
{
	funcA(0);

	outColor = vec4(inPosition, 1.0f);
}