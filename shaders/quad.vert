#version 330 core

in vec2 a_quad;
in vec2 a_ins_pos;

uniform int cols;

void main() {
	float s = 1 / float(cols);

	vec2 pos = a_quad;
	pos = pos * s;
	pos = pos + vec2(-1, 1) + vec2(s, s*-1);
	pos.x = pos.x + (s * 2 * a_ins_pos.x);
	pos.y = pos.y - (s * 2 * a_ins_pos.y);
	gl_Position = vec4(pos, 0.0f, 1.0f);
}