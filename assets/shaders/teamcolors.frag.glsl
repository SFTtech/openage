#version 120

// team color replacement shader
//
// replaces alpha hinted colors by team colors based on the player_number

// the unmodified texture itself
uniform sampler2D texture;

// the desired player number the final resulting colors
uniform int player_number;

// color entries for all players and their subcolors
uniform vec4 player_color[64];

// interpolated texture coordinates sent from vertex shader
varying vec2 tex_position;

// create epsilon environment for float comparison
const float epsilon = 0.001;

void main() {
	// get the texel from the uniform texture.
	vec4 pixel = texture2D(texture, tex_position);

	// recalcuate shade of the player color from alpha
	int subcolor = int(floor((pixel.a * 255.0 - 35.0) / 25));
	// get player color based on player and subcolor (shade)
	vec4 player_color = player_color[((player_number - 1) * 8) + subcolor];

	// check if it's a unit color
	float is_unit_color = floor(pixel.a + epsilon);

	// background is either transparent or the outline (which is nearly transparent)
	float is_unit_visible = float(pixel.a > 5.0 / 255.0 + epsilon);

	// assign correct color
	gl_FragColor = (pixel * is_unit_color + player_color * (1 - is_unit_color)) * is_unit_visible;
}
