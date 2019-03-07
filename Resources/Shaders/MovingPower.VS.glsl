#version 330

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;
layout(location = 3) in vec3 v_color;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float Time;
uniform float Alpha;
uniform int isON;

out vec3 frag_normal;
out vec3 frag_color;
out vec2 tex_coord;
out float time;

void main()
{
	frag_normal = v_normal;
	frag_color = v_color;
	tex_coord = v_texture_coord;
	time = (abs(sin(Time))/4);
	float x = sin(Alpha) * (abs(sin(Time))/4);
	float y = (abs(sin(Time)))/40;
	float z = cos(Alpha) * (abs(sin(Time))/4);
mat4 m = transpose(mat4(abs(sin(Time))*sin(Alpha)*3, 0,0,0,0,1,0,0,0,0,1,0,0,0,0,1));
	vec4 nou = Model * vec4(v_position, 1.0);
	//nou += vec4(x, y , z , 0);
	gl_Position = Projection * View * nou;
}
