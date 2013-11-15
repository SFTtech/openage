//total basic standard texture mapping vertex shader
#version 120

//per-vertex attributes for the texture coordinates
attribute vec2 base_texture_coordinates;
attribute vec2 mask_texture_coordinates;

varying vec2 base_texture_pos;
varying vec2 mask_texture_pos;

void main(void) {
	base_texture_pos = base_texture_coordinates;
	mask_texture_pos = mask_texture_coordinates;

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_FrontColor = gl_Color;
}
