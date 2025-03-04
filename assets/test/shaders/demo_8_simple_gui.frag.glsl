#version 330

in vec2 v_uv;
out vec4 fragColor;

void main() {
	// Semi-transparent white color for the GUI
	vec4 gui_color = vec4(1.0, 1.0, 1.0, 1.0);

	// Add a border to make it look more like a GUI window
	float border = 0.05;
	if (v_uv.x < border || v_uv.x > 1.0 - border || v_uv.y < border || v_uv.y > 1.0 - border) {
		gui_color = vec4(0.8, 0.8, 0.8, 1.0); // Darker border
	}

	fragColor = gui_color;
}
