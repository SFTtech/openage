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

//color entries for the teamcolor shader
const vec4 player_color[64] = vec4[64](

	//colors for player 1
	vec4(0.0/255.0, 0.0/255.0, 82.0/255.0, 1.0), //[0] = player 1 color 0
	vec4(0.0/255.0, 21.0/255.0, 130.0/255.0, 1.0), //[1] = player 1 color 1
	vec4(19.0/255.0, 49.0/255.0, 161.0/255.0, 1.0), //[2] = player 1 color 2
	vec4(48.0/255.0, 93.0/255.0, 182.0/255.0, 1.0), //[3] = player 1 color 3
	vec4(74.0/255.0, 121.0/255.0, 208.0/255.0, 1.0), //[4] = player 1 color 4
	vec4(110.0/255.0, 166.0/255.0, 235.0/255.0, 1.0), //[5] = player 1 color 5
	vec4(151.0/255.0, 206.0/255.0, 255.0/255.0, 1.0), //[6] = player 1 color 6
	vec4(205.0/255.0, 250.0/255.0, 255.0/255.0, 1.0), //[7] = player 1 color 7

	//colors for player 2
	vec4(65.0/255.0, 0.0/255.0, 0.0/255.0, 1.0), //[8] = player 2 color 0
	vec4(105.0/255.0, 11.0/255.0, 0.0/255.0, 1.0), //[9] = player 2 color 1
	vec4(160.0/255.0, 21.0/255.0, 0.0/255.0, 1.0), //[10] = player 2 color 2
	vec4(230.0/255.0, 11.0/255.0, 0.0/255.0, 1.0), //[11] = player 2 color 3
	vec4(255.0/255.0, 0.0/255.0, 0.0/255.0, 1.0), //[12] = player 2 color 4
	vec4(255.0/255.0, 100.0/255.0, 100.0/255.0, 1.0), //[13] = player 2 color 5
	vec4(255.0/255.0, 160.0/255.0, 160.0/255.0, 1.0), //[14] = player 2 color 6
	vec4(255.0/255.0, 220.0/255.0, 220.0/255.0, 1.0), //[15] = player 2 color 7

	//colors for player 3
	vec4(0.0/255.0, 0.0/255.0, 0.0/255.0, 1.0), //[16] = player 3 color 0
	vec4(0.0/255.0, 7.0/255.0, 0.0/255.0, 1.0), //[17] = player 3 color 1
	vec4(0.0/255.0, 32.0/255.0, 0.0/255.0, 1.0), //[18] = player 3 color 2
	vec4(0.0/255.0, 59.0/255.0, 0.0/255.0, 1.0), //[19] = player 3 color 3
	vec4(0.0/255.0, 87.0/255.0, 0.0/255.0, 1.0), //[20] = player 3 color 4
	vec4(0.0/255.0, 114.0/255.0, 0.0/255.0, 1.0), //[21] = player 3 color 5
	vec4(0.0/255.0, 141.0/255.0, 0.0/255.0, 1.0), //[22] = player 3 color 6
	vec4(0.0/255.0, 169.0/255.0, 0.0/255.0, 1.0), //[23] = player 3 color 7

	//colors for player 4
	vec4(80.0/255.0, 51.0/255.0, 26.0/255.0, 1.0), //[24] = player 4 color 0
	vec4(140.0/255.0, 78.0/255.0, 9.0/255.0, 1.0), //[25] = player 4 color 1
	vec4(191.0/255.0, 123.0/255.0, 0.0/255.0, 1.0), //[26] = player 4 color 2
	vec4(255.0/255.0, 199.0/255.0, 0.0/255.0, 1.0), //[27] = player 4 color 3
	vec4(255.0/255.0, 247.0/255.0, 37.0/255.0, 1.0), //[28] = player 4 color 4
	vec4(255.0/255.0, 255.0/255.0, 97.0/255.0, 1.0), //[29] = player 4 color 5
	vec4(255.0/255.0, 255.0/255.0, 166.0/255.0, 1.0), //[30] = player 4 color 6
	vec4(255.0/255.0, 255.0/255.0, 227.0/255.0, 1.0), //[31] = player 4 color 7

	//colors for player 5
	vec4(110.0/255.0, 23.0/255.0, 0.0/255.0, 1.0), //[32] = player 5 color 0
	vec4(150.0/255.0, 36.0/255.0, 0.0/255.0, 1.0), //[33] = player 5 color 1
	vec4(210.0/255.0, 55.0/255.0, 0.0/255.0, 1.0), //[34] = player 5 color 2
	vec4(255.0/255.0, 80.0/255.0, 0.0/255.0, 1.0), //[35] = player 5 color 3
	vec4(255.0/255.0, 130.0/255.0, 1.0/255.0, 1.0), //[36] = player 5 color 4
	vec4(255.0/255.0, 180.0/255.0, 21.0/255.0, 1.0), //[37] = player 5 color 5
	vec4(255.0/255.0, 210.0/255.0, 75.0/255.0, 1.0), //[38] = player 5 color 6
	vec4(255.0/255.0, 235.0/255.0, 160.0/255.0, 1.0), //[39] = player 5 color 7

	//colors for player 6
	vec4(0.0/255.0, 16.0/255.0, 16.0/255.0, 1.0), //[40] = player 6 color 0
	vec4(0.0/255.0, 37.0/255.0, 41.0/255.0, 1.0), //[41] = player 6 color 1
	vec4(0.0/255.0, 80.0/255.0, 80.0/255.0, 1.0), //[42] = player 6 color 2
	vec4(0.0/255.0, 120.0/255.0, 115.0/255.0, 1.0), //[43] = player 6 color 3
	vec4(0.0/255.0, 172.0/255.0, 150.0/255.0, 1.0), //[44] = player 6 color 4
	vec4(38.0/255.0, 223.0/255.0, 170.0/255.0, 1.0), //[45] = player 6 color 5
	vec4(109.0/255.0, 252.0/255.0, 191.0/255.0, 1.0), //[46] = player 6 color 6
	vec4(186.0/255.0, 255.0/255.0, 222.0/255.0, 1.0), //[47] = player 6 color 7

	//colors for player 7
	vec4(47.0/255.0, 0.0/255.0, 46.0/255.0, 1.0), //[48] = player 7 color 0
	vec4(79.0/255.0, 0.0/255.0, 75.0/255.0, 1.0), //[49] = player 7 color 1
	vec4(133.0/255.0, 12.0/255.0, 121.0/255.0, 1.0), //[50] = player 7 color 2
	vec4(170.0/255.0, 47.0/255.0, 155.0/255.0, 1.0), //[51] = player 7 color 3
	vec4(211.0/255.0, 58.0/255.0, 201.0/255.0, 1.0), //[52] = player 7 color 4
	vec4(241.0/255.0, 108.0/255.0, 232.0/255.0, 1.0), //[53] = player 7 color 5
	vec4(255.0/255.0, 169.0/255.0, 255.0/255.0, 1.0), //[54] = player 7 color 6
	vec4(255.0/255.0, 210.0/255.0, 255.0/255.0, 1.0), //[55] = player 7 color 7

	//colors for player 8
	vec4(28.0/255.0, 28.0/255.0, 28.0/255.0, 1.0), //[56] = player 8 color 0
	vec4(67.0/255.0, 67.0/255.0, 67.0/255.0, 1.0), //[57] = player 8 color 1
	vec4(106.0/255.0, 106.0/255.0, 106.0/255.0, 1.0), //[58] = player 8 color 2
	vec4(145.0/255.0, 145.0/255.0, 145.0/255.0, 1.0), //[59] = player 8 color 3
	vec4(185.0/255.0, 185.0/255.0, 185.0/255.0, 1.0), //[60] = player 8 color 4
	vec4(223.0/255.0, 223.0/255.0, 223.0/255.0, 1.0), //[61] = player 8 color 5
	vec4(247.0/255.0, 247.0/255.0, 247.0/255.0, 1.0), //[62] = player 8 color 6
	vec4(255.0/255.0, 255.0/255.0, 255.0/255.0, 1.0) //[63] = player 8 color 7
);



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
