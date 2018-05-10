//total basic standard texture drawing fragment shader

//the texture data
uniform sampler2D texture;

//interpolated texture coordinates received from vertex shader
varying vec2 tex_position;

void main (void) {
	//this sets the fragment color to the corresponding texel.
	gl_FragColor = texture2D(texture, tex_position);
}
