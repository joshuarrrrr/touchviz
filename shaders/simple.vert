#version 330 core

layout(location = 0) in vec2 pos;
layout(location = 1) in vec3 color;

out vec3 v_color;

void main() {
    gl_Position = vec4(vec2(-1.0 + pos.x * 2, 1.0 - pos.y * 2), 0.0, 1.0);
    v_color = color;
}