#version 400 core

flat in vec3 _normal;
flat in uint _color;
in vec3 _world_pos;

uniform vec3 u_light_pos;

out vec4 frag_color;

void main() {
	vec3 light_color = vec3(1, 1, 1);

	vec3 light_dir = normalize(u_light_pos - _world_pos);

	float diffuse_factor = max(dot(_normal, light_dir), 0.0);
	vec3 diffuse = diffuse_factor * light_color;

	float ambient_strength = 0.2;
	vec3 ambient = ambient_strength * light_color;

	vec4 color = unpackUnorm4x8(_color);

	frag_color = color * vec4(ambient + diffuse, 1.0);
}
