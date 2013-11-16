#version 120

//alpha masking shader
//
//applies an alpha mask texture to a base texture,
//then draws the masked texture.


uniform sampler2D base_texture;
uniform sampler2D mask_texture;

varying vec2 base_texture_pos;
varying vec2 mask_texture_pos;

varying vec4 colortest;

void main()
{
	//get the texel from the uniform texture.
	vec4 base_pixel = texture2D(base_texture, base_texture_pos);
	vec4 mask_pixel = texture2D(mask_texture, mask_texture_pos);

	//force to pink
	//base_pixel = vec4(255.0/255.0, 20.0/255.0, 147.0/255.0, 1.0);
	base_pixel = colortest;

	//vec4 draw_pixel = vec4(base_pixel.r, base_pixel.g, base_pixel.b, 0.5);
	gl_FragColor = base_pixel;
}
