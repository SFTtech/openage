#version 330

uniform sampler2D color_texture;

in vec2 v_uv;
out vec4 col;

void main() {
	col = texture(color_texture, v_uv);
}
