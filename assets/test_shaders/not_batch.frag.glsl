#version 330 core
out vec4 FragColor;

in vec4 ourColor;
in vec2 pos;
in vec2 auv;
uniform sampler2DArray texture_array;
uniform int layer = 0;
//uniform sampler2D texture_array;
void main()
{
	//float intensity = 100.0/(length(pos - mouse_pos));
	//FragColor = ourColor;//*intensity;
	FragColor = texture(texture_array,vec3(auv,layer));	
}
