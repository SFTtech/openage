#version 330

layout(location=0) in vec2 position;
layout(location=1) in vec2 uv;

out vec2 vert_uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
	gl_Position = proj * view * model * vec4(position, 0.0, 1.0);
    vert_uv = vec2(uv.x, 1.0 - uv.y);
}
