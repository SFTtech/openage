#version 120

//alpha masking shader
//
//applies an alpha mask texture to a base texture,
//then draws the masked texture.

//the base and mask texture, base is the plain terrain tile
uniform sampler2D texture;
uniform sampler2D mask_texture;

//disable blending and show the mask instead
uniform bool show_mask;

//get those interpolated texture position from vertexshader
varying vec2 base_tex_position;
varying vec2 mask_tex_position;


void main()
{
	//get the texel from the uniform texture.
	vec4 base_pixel = texture2D(texture, base_tex_position);
	vec4 mask_pixel = texture2D(mask_texture, mask_tex_position);

	float factor = 1.0 - mask_pixel.x;

	vec4 blended_pixel = vec4(base_pixel.r, base_pixel.g, base_pixel.b, base_pixel.a - factor);

	//force to pink
	//blended_pixel = vec4(255.0/255.0, 20.0/255.0, 147.0/255.0, 1.0);
	if (show_mask) {
		gl_FragColor = mask_pixel;
	} else {
		gl_FragColor = blended_pixel;
	}

	gl_FragDepth = 0;
}
