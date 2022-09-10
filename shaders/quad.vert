#version 400 core

in vec3 a_quad;
in vec3 a_normal;
in mat4 a_model_tx;
in uint a_color;

uniform mat4 u_proj_tx;
uniform mat4 u_view_tx;

flat out uint _color;
flat out vec3 _normal;
out vec3 _world_pos;

void main() {
	_world_pos = vec3(a_model_tx * vec4(a_quad, 1.0f));
	gl_Position =  u_proj_tx * u_view_tx * vec4(_world_pos, 1.0f);

	_color = a_color;
	_normal = normalize(a_normal);
}
