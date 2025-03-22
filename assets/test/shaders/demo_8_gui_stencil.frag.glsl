#version 330

in vec2 v_uv;
uniform float time; // Add time uniform for animation

out vec4 fragColor;

void main() {
	// Create moving gradient based on UV coordinates and time
	vec2 moving_uv = v_uv + vec2(sin(time * 0.5) * 0.5, cos(time * 0.3) * 0.5);

	// Create some colorful pattern
	vec3 color1 = vec3(0.2, 0.5, 0.8); // Blue-ish
	vec3 color2 = vec3(0.8, 0.2, 0.5); // Pink-ish

	vec3 color = mix(color1, color2, sin(moving_uv.x * 5.0 + time) * 0.5 + 0.5);
	color += vec3(0.2) * sin(moving_uv.y * 10.0 + time * 2.0);

	fragColor = vec4(color, 1.0);
}
