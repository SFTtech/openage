#version 120

//alpha masking shader
//
//applies an alpha mask texture to a base texture,
//then draws the masked texture.


uniform sampler2D base_texture;
uniform sampler2D mask_texture;


void main()
{
	//get the texel from the uniform texture.
	vec4 base_pixel = texture2D(base_texture, gl_TexCoord[0].xy);
	vec4 mask_pixel = texture2D(mask_texture, gl_TexCoord[0].xy);

	//force to pink
	//base_pixel = vec4(255.0/255.0, 20.0/255.0, 147.0/255.0, 1.0);

	gl_FragColor = base_pixel;
}
