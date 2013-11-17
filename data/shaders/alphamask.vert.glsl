//vertex shader for applying an alpha mask to a texture
#version 120


void main(void) {
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_TexCoord[1] = gl_MultiTexCoord1;
	gl_FrontColor = gl_Color;
}
