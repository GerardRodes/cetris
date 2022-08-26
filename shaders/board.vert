#version 330 core

in vec2 a_pos;

uniform mat4 u_proj_tx;
uniform mat4 u_board_tx;

void main() {
	gl_Position =  u_proj_tx * u_board_tx * vec4(a_pos, 0.0f, 1.0f);
}
