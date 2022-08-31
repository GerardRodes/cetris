#version 400 core

in vec3 a_quad;
in uvec2 a_ins_pos;
in uint a_ins_color;

uniform mat4 u_proj_tx;
uniform mat4 u_view_tx;
uniform mat4 u_model_tx;

flat out uint ins_color;

void main() {
	// todo: change by a transformation matrix
	vec3 quad = vec3(a_quad.x + a_ins_pos.x, a_quad.y + a_ins_pos.y, a_quad.z);
	gl_Position =  u_proj_tx * u_view_tx * u_model_tx * vec4(quad, 1.0f);

	ins_color = a_ins_color;
}
