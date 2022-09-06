#version 400 core

in vec2 a_quad;
in int a_col;
in int a_line;
in int a_ascii_code;

uniform mat4 u_proj_tx;

uniform float u_win_w;
uniform float u_win_h;
uniform float u_x;
uniform float u_y;
uniform float u_font_size;

out vec2 uv;

float tex_dim_xy = 16;
float char_w = 1.0 / tex_dim_xy;
float char_h = 1.0 / tex_dim_xy;

// use only 80% of the letter width
// makes text tighter
float letter_spacing = 0.8;

void main() {
	float x = u_x+(a_quad.x*u_font_size)+(a_col*u_font_size*letter_spacing);
	float y = u_y+(a_quad.y*u_font_size)+(a_line*u_font_size);
	gl_Position = u_proj_tx * vec4(x, y*(12.0/10.0), 0.0, 1.0);

	float u = (char_w*a_quad.x) + (char_w * mod(a_ascii_code, tex_dim_xy));
	float v = (char_h*a_quad.y) + (char_h * floor(a_ascii_code / tex_dim_xy));
	uv = vec2(u, v*-1);
}
