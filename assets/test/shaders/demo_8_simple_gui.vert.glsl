#version 330

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 uv;

out vec2 v_uv;

void main() {
	// Scale down the quad to show part of the background
	vec2 scaled_pos = position * 0.5; // Make it half size
	// Move it to top-right corner
	scaled_pos += vec2(0.5, 0.5);
	gl_Position = vec4(scaled_pos, 0.0, 1.0);
	v_uv = uv;
}
