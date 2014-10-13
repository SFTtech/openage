//vertex shader for applying an alpha mask to a texture
#version 120

//modelview*projection matrix
uniform mat4 mvp_matrix;

//the position of this vertex
attribute vec4 vertex_position;

//send the texture coordinates to the fragmentshader
attribute vec2 base_tex_coordinates;
attribute vec2 mask_tex_coordinates;

//send the texture coordinates to the fragmentshader
varying vec2 base_tex_position;
varying vec2 mask_tex_position;

void main(void) {
	//transform the position with the mvp matrix
	gl_Position = gl_ModelViewProjectionMatrix * vertex_position;

	//set the fixpoints for the tex coordinates at this vertex
	mask_tex_position = mask_tex_coordinates;
	base_tex_position = base_tex_coordinates;
}
