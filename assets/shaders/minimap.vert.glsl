#version 120

attribute vec2 vertex_position;

void main() {
  gl_Position = gl_ModelViewProjectionMatrix * vec4(vertex_position, 0, 0);
}
