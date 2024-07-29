#version 330

layout(location=0) in vec2 v_position;

void main() {
    // flip the y coordinate in OpenGL
    gl_Position = vec4(v_position.x, v_position.y, 0.0, 1.0);
}
