#version 120

attribute vec4 vertex_position;
attribute vec3 color;

varying vec4 out_vertex_position;
varying vec3 out_color;

void main() {
  out_vertex_position = vertex_position;
  out_color = color;
  gl_Position = gl_ModelViewProjectionMatrix * vertex_position;
}
