//team color replacement shader
//
//looks for an alpha value specified by 'alpha_marker'
//and then replaces this pixel with the desired color,
//tinted for player_number, and using the given color as base.


uniform sampler2D texture;
uniform int player_number;  //the desired player number the final resulting colors
uniform float alpha_marker; //the alpha value which marks colors to be replaced


const float epsilon = 0.001; //create epsilon environment for float comparison

/// player colors:

//colors for player 1
const vec4 player1_color_0 = vec4(0.0/255.0, 0.0/255.0, 82.0/255.0, 1.0);
const vec4 player1_color_1 = vec4(0.0/255.0, 21.0/255.0, 130.0/255.0, 1.0);
const vec4 player1_color_2 = vec4(19.0/255.0, 49.0/255.0, 161.0/255.0, 1.0);
const vec4 player1_color_3 = vec4(48.0/255.0, 93.0/255.0, 182.0/255.0, 1.0);
const vec4 player1_color_4 = vec4(74.0/255.0, 121.0/255.0, 208.0/255.0, 1.0);
const vec4 player1_color_5 = vec4(110.0/255.0, 166.0/255.0, 235.0/255.0, 1.0);
const vec4 player1_color_6 = vec4(151.0/255.0, 206.0/255.0, 255.0/255.0, 1.0);
const vec4 player1_color_7 = vec4(205.0/255.0, 250.0/255.0, 255.0/255.0, 1.0);
//colors for player 2
const vec4 player2_color_0 = vec4(65.0/255.0, 0.0/255.0, 0.0/255.0, 1.0);
const vec4 player2_color_1 = vec4(105.0/255.0, 11.0/255.0, 0.0/255.0, 1.0);
const vec4 player2_color_2 = vec4(160.0/255.0, 21.0/255.0, 0.0/255.0, 1.0);
const vec4 player2_color_3 = vec4(230.0/255.0, 11.0/255.0, 0.0/255.0, 1.0);
const vec4 player2_color_4 = vec4(255.0/255.0, 0.0/255.0, 0.0/255.0, 1.0);
const vec4 player2_color_5 = vec4(255.0/255.0, 100.0/255.0, 100.0/255.0, 1.0);
const vec4 player2_color_6 = vec4(255.0/255.0, 160.0/255.0, 160.0/255.0, 1.0);
const vec4 player2_color_7 = vec4(255.0/255.0, 220.0/255.0, 220.0/255.0, 1.0);
//colors for player 3
const vec4 player3_color_0 = vec4(0.0/255.0, 0.0/255.0, 0.0/255.0, 1.0);
const vec4 player3_color_1 = vec4(0.0/255.0, 7.0/255.0, 0.0/255.0, 1.0);
const vec4 player3_color_2 = vec4(0.0/255.0, 32.0/255.0, 0.0/255.0, 1.0);
const vec4 player3_color_3 = vec4(0.0/255.0, 59.0/255.0, 0.0/255.0, 1.0);
const vec4 player3_color_4 = vec4(0.0/255.0, 87.0/255.0, 0.0/255.0, 1.0);
const vec4 player3_color_5 = vec4(0.0/255.0, 114.0/255.0, 0.0/255.0, 1.0);
const vec4 player3_color_6 = vec4(0.0/255.0, 141.0/255.0, 0.0/255.0, 1.0);
const vec4 player3_color_7 = vec4(0.0/255.0, 169.0/255.0, 0.0/255.0, 1.0);
//colors for player 4
const vec4 player4_color_0 = vec4(80.0/255.0, 51.0/255.0, 26.0/255.0, 1.0);
const vec4 player4_color_1 = vec4(140.0/255.0, 78.0/255.0, 9.0/255.0, 1.0);
const vec4 player4_color_2 = vec4(191.0/255.0, 123.0/255.0, 0.0/255.0, 1.0);
const vec4 player4_color_3 = vec4(255.0/255.0, 199.0/255.0, 0.0/255.0, 1.0);
const vec4 player4_color_4 = vec4(255.0/255.0, 247.0/255.0, 37.0/255.0, 1.0);
const vec4 player4_color_5 = vec4(255.0/255.0, 255.0/255.0, 97.0/255.0, 1.0);
const vec4 player4_color_6 = vec4(255.0/255.0, 255.0/255.0, 166.0/255.0, 1.0);
const vec4 player4_color_7 = vec4(255.0/255.0, 255.0/255.0, 227.0/255.0, 1.0);
//colors for player 5
const vec4 player5_color_0 = vec4(110.0/255.0, 23.0/255.0, 0.0/255.0, 1.0);
const vec4 player5_color_1 = vec4(150.0/255.0, 36.0/255.0, 0.0/255.0, 1.0);
const vec4 player5_color_2 = vec4(210.0/255.0, 55.0/255.0, 0.0/255.0, 1.0);
const vec4 player5_color_3 = vec4(255.0/255.0, 80.0/255.0, 0.0/255.0, 1.0);
const vec4 player5_color_4 = vec4(255.0/255.0, 130.0/255.0, 1.0/255.0, 1.0);
const vec4 player5_color_5 = vec4(255.0/255.0, 180.0/255.0, 21.0/255.0, 1.0);
const vec4 player5_color_6 = vec4(255.0/255.0, 210.0/255.0, 75.0/255.0, 1.0);
const vec4 player5_color_7 = vec4(255.0/255.0, 235.0/255.0, 160.0/255.0, 1.0);
//colors for player 6
const vec4 player6_color_0 = vec4(0.0/255.0, 16.0/255.0, 16.0/255.0, 1.0);
const vec4 player6_color_1 = vec4(0.0/255.0, 37.0/255.0, 41.0/255.0, 1.0);
const vec4 player6_color_2 = vec4(0.0/255.0, 80.0/255.0, 80.0/255.0, 1.0);
const vec4 player6_color_3 = vec4(0.0/255.0, 120.0/255.0, 115.0/255.0, 1.0);
const vec4 player6_color_4 = vec4(0.0/255.0, 172.0/255.0, 150.0/255.0, 1.0);
const vec4 player6_color_5 = vec4(38.0/255.0, 223.0/255.0, 170.0/255.0, 1.0);
const vec4 player6_color_6 = vec4(109.0/255.0, 252.0/255.0, 191.0/255.0, 1.0);
const vec4 player6_color_7 = vec4(186.0/255.0, 255.0/255.0, 222.0/255.0, 1.0);
//colors for player 7
const vec4 player7_color_0 = vec4(47.0/255.0, 0.0/255.0, 46.0/255.0, 1.0);
const vec4 player7_color_1 = vec4(79.0/255.0, 0.0/255.0, 75.0/255.0, 1.0);
const vec4 player7_color_2 = vec4(133.0/255.0, 12.0/255.0, 121.0/255.0, 1.0);
const vec4 player7_color_3 = vec4(170.0/255.0, 47.0/255.0, 155.0/255.0, 1.0);
const vec4 player7_color_4 = vec4(211.0/255.0, 58.0/255.0, 201.0/255.0, 1.0);
const vec4 player7_color_5 = vec4(241.0/255.0, 108.0/255.0, 232.0/255.0, 1.0);
const vec4 player7_color_6 = vec4(255.0/255.0, 169.0/255.0, 255.0/255.0, 1.0);
const vec4 player7_color_7 = vec4(255.0/255.0, 210.0/255.0, 255.0/255.0, 1.0);
//colors for player 8
const vec4 player8_color_0 = vec4(28.0/255.0, 28.0/255.0, 28.0/255.0, 1.0);
const vec4 player8_color_1 = vec4(67.0/255.0, 67.0/255.0, 67.0/255.0, 1.0);
const vec4 player8_color_2 = vec4(106.0/255.0, 106.0/255.0, 106.0/255.0, 1.0);
const vec4 player8_color_3 = vec4(145.0/255.0, 145.0/255.0, 145.0/255.0, 1.0);
const vec4 player8_color_4 = vec4(185.0/255.0, 185.0/255.0, 185.0/255.0, 1.0);
const vec4 player8_color_5 = vec4(223.0/255.0, 223.0/255.0, 223.0/255.0, 1.0);
const vec4 player8_color_6 = vec4(247.0/255.0, 247.0/255.0, 247.0/255.0, 1.0);
const vec4 player8_color_7 = vec4(255.0/255.0, 255.0/255.0, 255.0/255.0, 1.0);

/// end of player colors

bool is_color(vec4 c1, vec4 reference) {
	if (all(greaterThanEqual(c1, reference - epsilon)) && all(lessThanEqual(c1, reference + epsilon))) {
		return true;
	}
	else {
		return false;
	}
}

vec4 get_color(int base, int playernumber) {
	if (playernumber == 1) {
		if (base == 0) {
			return player1_color_0;
		}
		else if (base == 1) {
			return player1_color_1;
		}
		else if (base == 2) {
			return player1_color_2;
		}
		else if (base == 3) {
			return player1_color_3;
		}
		else if (base == 4) {
			return player1_color_4;
		}
		else if (base == 5) {
			return player1_color_5;
		}
		else if (base == 6) {
			return player1_color_6;
		}
		else if (base == 7) {
			return player1_color_7;
		}
	}
	else if (playernumber == 2) {
		if (base == 0) {
			return player2_color_0;
		}
		else if (base == 1) {
			return player2_color_1;
		}
		else if (base == 2) {
			return player2_color_2;
		}
		else if (base == 3) {
			return player2_color_3;
		}
		else if (base == 4) {
			return player2_color_4;
		}
		else if (base == 5) {
			return player2_color_5;
		}
		else if (base == 6) {
			return player2_color_6;
		}
		else if (base == 7) {
			return player2_color_7;
		}
	}
	else if (playernumber == 3) {
		if (base == 0) {
			return player3_color_0;
		}
		else if (base == 1) {
			return player3_color_1;
		}
		else if (base == 2) {
			return player3_color_2;
		}
		else if (base == 3) {
			return player3_color_3;
		}
		else if (base == 4) {
			return player3_color_4;
		}
		else if (base == 5) {
			return player3_color_5;
		}
		else if (base == 6) {
			return player3_color_6;
		}
		else if (base == 7) {
			return player3_color_7;
		}
	}
	else if (playernumber == 4) {
		if (base == 0) {
			return player4_color_0;
		}
		else if (base == 1) {
			return player4_color_1;
		}
		else if (base == 2) {
			return player4_color_2;
		}
		else if (base == 3) {
			return player4_color_3;
		}
		else if (base == 4) {
			return player4_color_4;
		}
		else if (base == 5) {
			return player4_color_5;
		}
		else if (base == 6) {
			return player4_color_6;
		}
		else if (base == 7) {
			return player4_color_7;
		}
	}
	else if (playernumber == 5) {
		if (base == 0) {
			return player5_color_0;
		}
		else if (base == 1) {
			return player5_color_1;
		}
		else if (base == 2) {
			return player5_color_2;
		}
		else if (base == 3) {
			return player5_color_3;
		}
		else if (base == 4) {
			return player5_color_4;
		}
		else if (base == 5) {
			return player5_color_5;
		}
		else if (base == 6) {
			return player5_color_6;
		}
		else if (base == 7) {
			return player5_color_7;
		}
	}
	else if (playernumber == 6) {
		if (base == 0) {
			return player6_color_0;
		}
		else if (base == 1) {
			return player6_color_1;
		}
		else if (base == 2) {
			return player6_color_2;
		}
		else if (base == 3) {
			return player6_color_3;
		}
		else if (base == 4) {
			return player6_color_4;
		}
		else if (base == 5) {
			return player6_color_5;
		}
		else if (base == 6) {
			return player6_color_6;
		}
		else if (base == 7) {
			return player6_color_7;
		}
	}
	else if (playernumber == 7) {
		if (base == 0) {
			return player7_color_0;
		}
		else if (base == 1) {
			return player7_color_1;
		}
		else if (base == 2) {
			return player7_color_2;
		}
		else if (base == 3) {
			return player7_color_3;
		}
		else if (base == 4) {
			return player7_color_4;
		}
		else if (base == 5) {
			return player7_color_5;
		}
		else if (base == 6) {
			return player7_color_6;
		}
		else if (base == 7) {
			return player7_color_7;
		}
	}
	else if (playernumber == 8) {
		if (base == 0) {
			return player8_color_0;
		}
		else if (base == 1) {
			return player8_color_1;
		}
		else if (base == 2) {
			return player8_color_2;
		}
		else if (base == 3) {
			return player8_color_3;
		}
		else if (base == 4) {
			return player8_color_4;
		}
		else if (base == 5) {
			return player8_color_5;
		}
		else if (base == 6) {
			return player8_color_6;
		}
		else if (base == 7) {
			return player8_color_7;
		}
	}
	return vec4(1.0, 0.0, 0.0, 1.0);
}



void main()
{
	vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);
	float replacement_alpha = alpha_marker;

	if (pixel[3] >= replacement_alpha - epsilon && pixel[3] <= replacement_alpha + epsilon) {

		pixel[3] = 1.0; //set it for the comparison

		//dont replace the color if it's already player 1 (blue)
		if (player_number != 1) {
			if (is_color(pixel, player1_color_0)) {
				pixel = get_color(0, player_number);
			}
			else if (is_color(pixel, player1_color_1)) {
				pixel = get_color(1, player_number);
			}
			else if (is_color(pixel, player1_color_2)) {
				pixel = get_color(2, player_number);
			}
			else if (is_color(pixel, player1_color_3)) {
				pixel = get_color(3, player_number);
			}
			else if (is_color(pixel, player1_color_4)) {
				pixel = get_color(4, player_number);
			}
			else if (is_color(pixel, player1_color_5)) {
				pixel = get_color(5, player_number);
			}
			else if (is_color(pixel, player1_color_6)) {
				pixel = get_color(6, player_number);
			}
			else if (is_color(pixel, player1_color_7)) {
				pixel = get_color(7, player_number);
			}
			else {
				//unknown base color gets pink muhahaha
				pixel = vec4(255.0/255.0, 20.0/255.0, 147.0/255.0, 1.0);
			}
		}
	}

	gl_FragColor = pixel;
}
