#version 330

layout(location=0) in vec2 position;
layout(location=1) in vec2 uv;
uniform mat4 proj;
out vec2 v_uv;

void main() {
	gl_Position = proj * vec4(position, 0.0, 1.0);
	v_uv = uv;
}
