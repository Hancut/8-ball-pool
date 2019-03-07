#version 330

in float time;

layout(location = 0) out vec4 out_color;

void main()
{
	out_color = vec4(time * 3, 1 - time*3, 0, 1);
}