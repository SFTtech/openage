#version 330 core

uniform sampler2D tex;

in vec2 tex_position;
out vec4 frag_color;

void main(void) {
    frag_color = texture(tex, tex_position);
}
