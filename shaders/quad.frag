#version 400 core

flat in uint ins_color;

out vec4 color;

void main() {
	color = unpackUnorm4x8(ins_color);
}
