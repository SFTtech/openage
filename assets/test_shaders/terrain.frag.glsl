#version 330 core
out vec4 FragColor;

in vec2 pos;
in vec2 auv;
in float tex_ind;
uniform sampler2DArray texture_array;
void main()
{
	FragColor = texture(texture_array,vec3(auv,tex_ind));	
}
