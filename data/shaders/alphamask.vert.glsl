//vertex shader for applying an alpha mask to a texture
#version 120

//uniform mat4 projection_matrix;
//uniform mat4 modelview_matrix;

//attributes for the texture coordinates
attribute vec2 base_tex_coords;
attribute vec2 mask_tex_coords;

//the position in space of this vertex
attribute vec4 position;

//the interpolated fragment positions for both textures
varying vec2 base_texture_pos;
varying vec2 mask_texture_pos;

varying vec4 colortest;

void main(void) {
	base_texture_pos = base_tex_coords;
	mask_texture_pos = mask_tex_coords;

	if (position.x < 0) {
		colortest = vec4(1.0, 0.0, 1.0, 1.0);
	}
	else {
		colortest = vec4(0.0, 0.0, 0.0, 1.0);
	}

	//gl_Position = projection_matrix * modelview_matrix * gl_Vertex;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	//gl_Position = gl_ModelViewProjectionMatrix * position;

	gl_FrontColor = gl_Color;
}
