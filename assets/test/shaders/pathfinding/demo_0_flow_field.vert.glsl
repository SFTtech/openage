#version 330

layout(location=0) in vec3 position;
layout(location=1) in float flow_cell;
layout(location=2) in float int_cell;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out float flow_val;
out float int_val;

void main() {
	gl_Position = proj * view * model * vec4(position, 1.0);
    flow_val = flow_cell;
    int_val = int_cell;
}
