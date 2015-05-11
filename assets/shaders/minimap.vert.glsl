#version 120

attribute vec4 vertex_position;

void main() {
  gl_Position = gl_ModelViewProjectionMatrix * vertex_position;
}
