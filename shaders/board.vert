#version 400 core

in vec2 a_pos;
in vec3 a_col;

uniform mat4 u_proj_tx;
uniform mat4 u_view_tx;
uniform mat4 u_model_tx;

out vec3 col;

void main() {
	gl_Position =  u_proj_tx * u_view_tx * u_model_tx * vec4(a_pos, 0.0f, 1.0f);

	col = a_col;
}
