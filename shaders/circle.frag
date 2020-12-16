#version 330 core

in vec3 v_color;
out vec4 frag_color;

float aastep(float threshold, float value) {
  float afwidth = length(vec2(dFdx(value), dFdy(value))) * 0.70710678118654757;
  return smoothstep(threshold - afwidth, threshold + afwidth, value);
}

float circle(vec2 st, float radius) {
  return aastep(radius, length(st - vec2(0.5)));
}

void main() {
  float alpha = 1 - circle(gl_PointCoord, 0.48);
  frag_color = vec4(v_color, alpha);
}