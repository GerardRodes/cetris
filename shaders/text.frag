#version 400 core

in vec2 _uv;

uniform sampler2D u_tex0;
uniform uint u_bg_color;
uniform uint u_fg_color;

out vec4 frag_color;

void main() {
	frag_color = texture(u_tex0, _uv);
	vec4 p = texture(u_tex0, _uv);
	if (p.r == 0.0 && p.g == 0.0 && p.b == 0.0) {
		frag_color = unpackUnorm4x8(u_bg_color);
	} else {
		frag_color = unpackUnorm4x8(u_fg_color);
	}
}
