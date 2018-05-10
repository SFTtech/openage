//team color replacement shader
//
//looks for an alpha value specified by 'alpha_marker'
//and then replaces this pixel with the desired color,
//tinted for player_number, and using the given color as base.

//the unmodified texture itself
uniform sampler2D texture;

//the desired player number the final resulting colors
uniform int player_number;

//the alpha value which marks colors to be replaced
uniform float alpha_marker;

//color entries for all players and their subcolors
uniform vec4 player_color[NUM_OF_PLAYER_COLORS];

//interpolated texture coordinates sent from vertex shader
varying vec2 tex_position;

//create epsilon environment for float comparison
const float EPSILON = 0.001;

//do the lookup in the player color table
//for a playernumber (red, blue, etc)
//get the subcolor (brightness variations)
vec4 get_color(int playernum, int subcolor) {
	return player_color[((playernum-1) * 8) + subcolor];
}

void main() {
	//get the texel from the uniform texture.
	vec4 pixel = texture2D(texture, tex_position);

	//check if this texel has an alpha marker, so we can replace it's rgb values.
	if (player_number != 1 && equalsEpsilon(pixel[3], alpha_marker, EPSILON)) {
		//try to find the base color, there are 8 of them.
		for(int i = 0; i <= 7; i++) {
			if (equalsEpsilon(vec3(pixel), vec3(player_color[i]), EPSILON)) {
				//base color found, now replace it with the same color
				//but player_number tinted.
				gl_FragColor = get_color(player_number, i);
				return;
			}
		}

		//unknown base color gets pink muhahaha
		pixel = vec4(255.0/255.0, 20.0/255.0, 147.0/255.0, 1.0);
	}

	//else the texel had no marker so we can just draw it without player coloring
	gl_FragColor = pixel;
}
