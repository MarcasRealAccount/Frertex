#version 460 core

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec4 inNormal;

layout(location = 0) out vec2 outUV;

void main()
{
	uint v = gl_VertexIndex;

	gl_Position = vec4(inPosition.xyz, 1.0f);
	outUV       = vec2(inPosition.w + float(v), inNormal.w);
}