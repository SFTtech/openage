#version 120

//team color replacement shader
//
//looks for an alpha value specified by 'alpha_marker'
//and then replaces this pixel with the desired color,
//tinted for player_number, and using the given color as base.

//the unmodified texture itself
uniform sampler2D texture;

//the alpha value which marks colors to be replaced
uniform float alpha_marker;

//color entries for all players and their subcolors
uniform vec4 player_color[64];

//interpolated texture coordinates sent from vertex shader
varying vec2 tex_position;

//the desired player number the final resulting colors
varying float vplayer_number;
varying float vz_order;

//create epsilon environment for float comparison
const float epsilon = 0.001;
const vec3 vepsilon = vec3(epsilon, epsilon, epsilon);


void main() {
	//get the texel from the uniform texture.
	vec4 pixel = texture2D(texture, tex_position);

	if ( abs(pixel.a - alpha_marker) > epsilon )
	{
		//Even replace player 1's colours as we then remove a branch
		for(int i = 0; i <= 7; i++) {
			if ( all(lessThanEqual(abs(pixel.rgb - player_color[i].rgb), vepsilon)) )
			{
				pixel = player_color[int( ((vplayer_number-1) * 8) + i )];
				//pixel = vec4(1, 1, 1, 1);
				break;
			} 
		}
	}

	gl_FragColor = pixel;
	gl_FragDepth = 0.2;
	//gl_FragDepth = floor(gl_FragColor.a) * 0.2;
}