// alpha masking shader
//
// applies an alpha mask texture to a base texture,
// then draws the masked texture.
#version 330

// the base and mask texture, base is the plain terrain tile
uniform sampler2D base_texture;
uniform sampler2D mask_texture;

// disable blending and show the mask instead
//uniform bool show_mask;

// get those interpolated texture position from vertexshader
in vec2 base_tex_position;
in vec2 mask_tex_position;

layout(location=0) out vec4 blended_pixel;

void main() {
	// get the texel from the uniform texture.
	vec4 base_pixel = texture2D(base_texture, base_tex_position);
	vec4 mask_pixel = texture2D(mask_texture, mask_tex_position);

	//float factor = 1.0 - mask_pixel.x;
	float factor = mask_pixel.x;
	vec4 blended_pixel = vec4(base_pixel.r, base_pixel.g, base_pixel.b, base_pixel.a - factor);

	//if (0) {
	//	gl_FragColor = mask_pixel;
	//} else {
		gl_FragColor = blended_pixel;
	//}
}
