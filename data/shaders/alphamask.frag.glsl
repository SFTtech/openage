#version 120

//alpha masking shader
//
//applies an alpha mask texture to a base texture,
//then draws the masked texture.

uniform sampler2D base_texture;
uniform sampler2D mask_texture;

uniform bool show_mask;

void main()
{
	//get the texel from the uniform texture.
	vec4 base_pixel = texture2D(base_texture, gl_TexCoord[0].xy);
	vec4 mask_pixel = texture2D(mask_texture, gl_TexCoord[1].xy);

	float factor = 1.0 - mask_pixel.x;

	vec4 blended_pixel = vec4(base_pixel.r, base_pixel.g, base_pixel.b, base_pixel.a - factor);

	//force to pink
	//base_pixel = vec4(255.0/255.0, 20.0/255.0, 147.0/255.0, 1.0);
	if (show_mask) {
		gl_FragColor = mask_pixel;
	} else {
		gl_FragColor = blended_pixel;
	}
}
