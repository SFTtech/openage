// vertex shader for applying an alpha mask to a texture
#version 330

// modelview*projection matrix
uniform mat4 mvp_matrix;

// the position of this vertex
layout(location=0) in vec2 vertex_position;

// send the texture coordinates to the fragmentshader
layout(location=1) in vec2 base_tex_coordinates;
layout(location=2) in vec2 mask_tex_coordinates;

// send the texture coordinates to the fragmentshader
out vec2 base_tex_position;
out vec2 mask_tex_position;

void main(void) {
	// transform the position with the mvp matrix
	gl_Position = mvp_matrix * vec4(vertex_position,0.0,1.0);

	// set the fixpoints for the tex coordinates at this vertex
	mask_tex_position = mask_tex_coordinates;
	base_tex_position = base_tex_coordinates;
}
