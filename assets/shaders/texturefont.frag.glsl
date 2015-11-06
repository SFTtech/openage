#version 120

varying vec2 tex_position;
uniform sampler2D texture;
uniform vec4 color;

void main() {
	// Glyph's image data is stored in the RED channel of the texture
	float red = texture2D(texture, tex_position).r;
	if (red < 0.01) {
		discard;
	}

	gl_FragColor = color * vec4(1.0f, 1.0f, 1.0f, red);
}
