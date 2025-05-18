#version 330 core

uniform sampler2D tex_sampler;

in vec2 tex_position;
out vec4 frag_color;

void main(void) {
    frag_color = texture(tex_sampler, tex_position);
}
