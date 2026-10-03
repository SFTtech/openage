#version 330 core

// Input vertex position
layout(location = 0) in vec4 vertex_position;

// Output to fragment shader
out vec2 tex_position;

void main(void) {
    gl_Position = vertex_position;

    // Convert from clip space (-1, 1) to texture space (0, 1)
    tex_position = (vertex_position.xy + vec2(1.0)) * 0.5;
}
