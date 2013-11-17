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
varying out vec2 base_texture_pos;
varying out vec2 mask_texture_pos;

varying out vec4 colortest;

void main(void) {
	base_texture_pos = btexc.xy;
	mask_texture_pos = mtexc.xy;

	if (vposition.x == gl_Vertex.x) {
		colortest = vec4(1.0, 0.0, 1.0, 1.0);
	}
	else {
		colortest = vec4(0.0, 1.0, 0.0, 1.0);
	}

	//gl_Position = projection_matrix * modelview_matrix * vposition;
	gl_Position = gl_ModelViewProjectionMatrix * vposition; //gl_Vertex

	gl_FrontColor = gl_Color;
}
