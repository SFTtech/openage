uniform sampler2D texture;
uniform vec4 player_number;
uniform float alpha_marker;

void main()
{
	vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);

	float epsilon = 0.009;
	if (greaterThanEqual(pixel[4], alpha_marker - epsilon) && lessThanEqual(pixel[4], alpha_marker + epsilon)) {
		// TODO: calculate the real player color with base color "pixel"
		// use player_number * 16 + pixel  somehow..

		// for now, some pink
		pixel = vec4(255/255, 20/255, 147/255, 1.0)
	}

	gl_FragColor = pixel;
}
