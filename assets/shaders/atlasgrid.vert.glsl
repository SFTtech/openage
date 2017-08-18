#version 120

attribute highp vec4 vertex;
attribute lowp vec4 color;
uniform highp mat4 qt_Matrix;

// ID in a square atlas grid with number of columns 'columns'.
attribute highp float cell_id;

// Pass as is to the fragment shader.
varying vec4 v_color;
varying float v_cell_id;

void main() {
	gl_Position = qt_Matrix * vertex;
	v_color = color;
	v_cell_id = cell_id;
}
