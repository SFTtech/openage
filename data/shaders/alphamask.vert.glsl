//vertex shader for applying an alpha mask to a texture
#version 120


uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;

//the position in space of this vertex
in vec4 vposition;

//attributes for the texture coordinates
in vec4 btexc;
in vec4 mtexc;

//the interpolated fragment positions for both textures
varying vec2 base_texture_pos;
varying vec2 mask_texture_pos;

varying vec4 colortest;

void main(void) {
	base_texture_pos = btexc.xy;
	mask_texture_pos = mtexc.xy;

	if (gl_Vertex.x == 0) {}

	colortest = vec4(0.0, 1.0, 0.0, 1.0);

	//gl_Position = projection_matrix * modelview_matrix * vposition;
	gl_Position = gl_ModelViewProjectionMatrix * vposition; //gl_Vertex

	gl_FrontColor = gl_Color;
}
