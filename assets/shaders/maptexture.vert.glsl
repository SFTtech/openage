//total basic standard texture mapping vertex shader
#version 120

//modelview*projection matrix
uniform mat4 mvp_matrix;

//the position of this vertex
attribute vec4 vertex_position;

//the texture coordinates assigned to this vertex
attribute vec2 tex_coordinates;

attribute float player_number;

//interpolated texture coordinates sent to fragment shader
varying vec2 tex_position;
varying float vplayer_number;

void main(void) {
	//transform the vertex coordinates
	gl_Position = gl_ModelViewProjectionMatrix * vertex_position;

	//pass the fix points for texture coordinates set at this vertex
	tex_position = tex_coordinates;
	vplayer_number = player_number;	
}
