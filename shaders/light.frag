#version 400 core

in vec3 _pos;
in float _point_size;

uniform mat4 u_proj_tx;
uniform mat4 u_view_tx;

out vec4 frag_color;

void main() {
  vec2 st = gl_FragCoord.xy/vec2(_point_size/2);
  gl_FragColor = vec4(1, 1, 1, distance(st, vec2(0.5)));
}
