//total basic standard texture mapping vertex shader
#version 120

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;

//per-vertex attributes for the texture coordinates
attribute vec2 base_texture_coordinates;
attribute vec2 mask_texture_coordinates;

attribute vec4 vposition;

varying vec2 base_texture_pos;
varying vec2 mask_texture_pos;

void main(void) {
	base_texture_pos = base_texture_coordinates;
	mask_texture_pos = mask_texture_coordinates;

	gl_Position = gl_ModelViewProjectionMatrix * vposition; //projection_matrix * modelview_matrix * gl_Vertex;
	gl_FrontColor = gl_Color;
}
