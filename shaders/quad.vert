#version 400 core

in vec3 a_quad;
in mat4 a_model_tx;
in uint a_ins_color;

uniform mat4 u_proj_tx;
uniform mat4 u_view_tx;

flat out uint ins_color;

void main() {
	gl_Position =  u_proj_tx * u_view_tx * a_model_tx * vec4(a_quad, 1.0f);

	ins_color = a_ins_color;
}
