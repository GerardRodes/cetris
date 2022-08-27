#version 400 core

in vec2 a_quad;
in vec2 a_ins_pos;
in uint a_ins_color;

uniform mat4 u_proj_tx;
uniform mat4 u_board_tx;

flat out uint ins_color;

void main() {
	vec2 quad = vec2(a_quad.x + a_ins_pos.x, a_quad.y + a_ins_pos.y);
	gl_Position =  u_proj_tx * u_board_tx * vec4(quad, 0.0f, 1.0f);

	ins_color = a_ins_color;
}
