#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

out vec2 tex_pos;

uniform mat4 model;

layout (std140) uniform cam {
    mat4 view;
    mat4 proj;
};

void main() {
	gl_Position = proj * view * model * vec4(position, 1.0);
    tex_pos = vec2(uv.x, 1.0 - uv.y);
}
