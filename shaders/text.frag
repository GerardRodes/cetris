#version 400 core

in vec4 color; // font texture

uniform uint u_bg_color;
uniform uint u_fg_color;

out vec4 frag_color;

void main() {
	if (color.r == 0.0 && color.g == 0.0 && color.b == 0.0) {
		frag_color = unpackUnorm4x8(u_bg_color);
	} else {
		frag_color = unpackUnorm4x8(u_fg_color);
	}
}
