//total basic standard texture mapping fragment shader

uniform sampler2D texure;

void main (void) {
	//this sets the fragment color to the corresponding texel.
	gl_FragColor = texture2D(texture, gl_TexCoord[0].st);
	//gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0); //force it to blue
}