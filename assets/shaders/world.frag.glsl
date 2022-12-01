#version 330

in vec2 tex_pos;

out vec4 out_col;

uniform sampler2D tex;

void main() {
	out_col = texture(tex, tex_pos);
}
