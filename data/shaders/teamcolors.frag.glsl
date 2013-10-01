#version 120

//team color replacement shader
//
//looks for an alpha value specified by 'alpha_marker'
//and then replaces this pixel with the desired color,
//tinted for player_number, and using the given color as base.


uniform sampler2D texture;
uniform int player_number;  //the desired player number the final resulting colors
uniform float alpha_marker; //the alpha value which marks colors to be replaced


const float epsilon = 0.001; //create epsilon environment for float comparison

//color entries for all players and their subcolors
uniform vec4 player_color[64];


vec4 get_color(int playernum, int subcolor) {
	return player_color[((playernum-1) * 8) + subcolor];
}


bool is_color(vec4 c1, vec4 reference) {
	if (all(greaterThanEqual(c1, reference - epsilon)) && all(lessThanEqual(c1, reference + epsilon))) {
		return true;
	}
	else {
		return false;
	}
}


void main()
{
	vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);
	float replacement_alpha = alpha_marker;

	if (pixel[3] >= replacement_alpha - epsilon && pixel[3] <= replacement_alpha + epsilon) {

		pixel[3] = 1.0; //set it for the comparison

		//dont replace the color if it's already player 1 (blue)
		if (player_number != 1) {
			if (is_color(pixel, player_color[0])) {
				pixel = get_color(player_number, 0);
			}
			else if (is_color(pixel, player_color[1])) {
				pixel = get_color(player_number, 1);
			}
			else if (is_color(pixel, player_color[2])) {
				pixel = get_color(player_number, 2);
			}
			else if (is_color(pixel, player_color[3])) {
				pixel = get_color(player_number, 3);
			}
			else if (is_color(pixel, player_color[4])) {
				pixel = get_color(player_number, 4);
			}
			else if (is_color(pixel, player_color[5])) {
				pixel = get_color(player_number, 5);
			}
			else if (is_color(pixel, player_color[6])) {
				pixel = get_color(player_number, 6);
			}
			else if (is_color(pixel, player_color[7])) {
				pixel = get_color(player_number, 7);
			}
			else {
				//unknown base color gets pink muhahaha
				pixel = vec4(255.0/255.0, 20.0/255.0, 147.0/255.0, 1.0);
			}
		}
	}

	gl_FragColor = pixel;
}
