#version 330

layout (location = 0) in vec3 position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
	gl_Position = proj * view * model * vec4(position, 1.0);
}
