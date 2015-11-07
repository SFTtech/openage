#version 120

attribute vec4 vertex_position;
attribute vec2 tex_coordinates;
varying vec2 tex_position;

void main() {
	gl_Position = gl_ModelViewProjectionMatrix * vertex_position;
	tex_position = tex_coordinates;
}
