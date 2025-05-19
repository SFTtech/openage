#version 330 core

// Input attributes (set via glVertexAttribPointer)
layout(location = 0) in vec4 vertex_position;
layout(location = 1) in vec2 tex_coordinates;

// Output to fragment shader
out vec2 tex_position;

// Uniform matrix
uniform mat4 mvp_matrix;

void main(void) {
    gl_Position = mvp_matrix * vertex_position;
    tex_position = tex_coordinates;
}
