#version 330 core
out vec4 FragColor;

in vec4 ourColor;
in vec2 pos;
in vec2 auv;
//uniform vec2 mouse_pos = vec2(0.0,0.0);
uniform sampler2D texture_0;
void main()
{
	//float intensity = 100.0/(length(pos - mouse_pos));
	//FragColor = ourColor;//*intensity;
	FragColor = texture(texture_0,auv);	
}
