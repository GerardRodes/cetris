#version 330 core

in vec2 a_quad;
in vec2 a_ins_pos;

uniform mat4 u_proj_tx;
uniform mat4 u_board_tx;

void main() {
	vec2 quad = vec2(a_quad.x + a_ins_pos.x, a_quad.y + a_ins_pos.y);
	gl_Position =  u_proj_tx * u_board_tx * vec4(quad, 0.0f, 1.0f);
}
