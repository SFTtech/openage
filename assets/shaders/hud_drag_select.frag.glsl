#version 330

// Color of the drag rectangle
uniform vec4 in_col;

layout(location=0) out vec4 out_col;

void main() {
	out_col = in_col;
}
