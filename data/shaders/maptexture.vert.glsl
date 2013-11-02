//total basic standard texture mapping vertex shader
#version 120

void main(void) {
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_FrontColor = gl_Color;
}
