//team color replacement shader
//
//looks for an alpha value specified by 'alpha_marker'
//and then replaces this pixel with the desired color,
//tinted for player_number, and using the given color as base.


uniform sampler2D texture;
uniform int player_number;  //the desired player number the final resulting colors
uniform float alpha_marker; //the alpha value which marks colors to be replaced

void main()
{
	vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);
	float replacement_alpha = alpha_marker;
	replacement_alpha = 254.0/255.0; //hardcoded for now.

	//create epsilon environment for float comparison
	float epsilon = 0.001;
	if (pixel[3] >= replacement_alpha - epsilon && pixel[3] <= replacement_alpha + epsilon) {
		// TODO: calculate the real player color with base color "pixel"
		// use player_number * 16 + pixel  somehow..

		// for now, some pink
		pixel = vec4(255.0/255.0, 20.0/255.0, 147.0/255.0, 1.0);
	}

	gl_FragColor = pixel;
}
