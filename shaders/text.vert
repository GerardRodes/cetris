#version 400 core

in vec2 a_quad;
in int a_col;
in int a_line;
in int a_ascii_code;

uniform uint u_x;
uniform uint u_y;
uniform uint u_font_size;

void main() {
	// todo: load texture
	// a_ascii_code
	gl_Position = vec4(u_x + a_col, u_y + a_line, 1.0, 1.0);
}
