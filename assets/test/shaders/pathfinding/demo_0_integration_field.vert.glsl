#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in float cost;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out float v_cost;

void main() {
	gl_Position = proj * view * model * vec4(position, 1.0);
    v_cost = cost;
}
