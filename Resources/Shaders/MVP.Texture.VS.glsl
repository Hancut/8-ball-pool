#version 330

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

// Uniforms for light properties
uniform vec3 light_position;
uniform vec3 eye_position;
uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;
uniform vec3 object_color;

// Output value to fragment shader
out vec3 color;

void main()
{
// TODO: compute world space vectors
	vec3 world_pos = (Model * vec4(v_position, 1)).xyz;
	vec3 world_normal = normalize(mat3(Model) * normalize(v_normal));

	vec3 L = normalize( light_position - world_pos );
	vec3 V = normalize( eye_position - world_pos );
	vec3 H = normalize( L + V );

	// TODO: define ambient light component
	float ambient_light = material_kd * 0.60;

	// TODO: compute diffuse light component
	float diffuse_light = max(dot(L, world_normal), 0) * material_kd;

	// TODO: compute specular light component
	float specular_light = 0;

	//if (diffuse_light > 0)
	//{
		specular_light =  material_ks * pow(max(dot(world_normal, H), 0), material_shininess);
	//}

	// TODO: compute light
	float d = distance(light_position, world_pos);
	float FactorAtenuare = 1/(d*d);
	float I = ambient_light + (diffuse_light + specular_light)*FactorAtenuare;

	// TODO: send color light output to fragment shader
	color = object_color * I;
	gl_Position = Projection * View * Model * vec4(v_position, 1.0);
}
