#version 400 core

flat in vec3 _normal;
flat in uint _color;

out vec4 frag_color;

void main() {
	float ambient_strength = 0.1 * _normal.x;
	vec3 light_color = vec3(0.8, 0.2, 0.5);
	vec3 ambient = ambient_strength * light_color;
	vec4 color = unpackUnorm4x8(_color);
	frag_color = color * vec4(ambient, 1.0);
}
