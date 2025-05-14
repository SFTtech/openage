#version 330

in vec2 position;
out vec2 tex_coord;

void main() {
	tex_coord = position.xy * 0.5 + 0.5;
	gl_Position = vec4(position.xy, 0.0, 1.0);
}
