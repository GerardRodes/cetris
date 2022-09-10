#version 400 core

in vec3 a_pos;

uniform mat4 u_proj_tx;
uniform mat4 u_view_tx;

out vec3 _pos;
out float _point_size;

void main() {
	vec4 view_space = u_view_tx * vec4(a_pos, 1.0f);
	gl_Position =  u_proj_tx * view_space;
	gl_PointSize = 100 + (view_space.z * 10);

	_pos = a_pos;
	_point_size = gl_PointSize;
}
