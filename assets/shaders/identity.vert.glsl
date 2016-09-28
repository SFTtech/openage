//no-transformation texture mapping vertex shader

//the position of this vertex
attribute vec4 vertex_position;

//interpolated texture coordinates sent to fragment shader
varying vec2 tex_position;

void main(void) {
	gl_Position = vertex_position;

	//convert from screen coordinates (-1, 1) to texture coordinates (0, 1)
	tex_position = (vertex_position.xy + vec2(1.f)) / 2.f;
}
