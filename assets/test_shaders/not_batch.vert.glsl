#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 uv;
uniform mat4 ortho = mat4(1.0); 
/*mat4 ortho = mat4(2.0/1920.0,0.0,0.0,-1.0,
		0.0,-2.0/1080.0,0.0,1.0,
		0.0,0.0,1.0,0.0,
		0.0,0.0,1.0,1.0);
*/
out vec4 ourColor;
out vec2 pos;
out vec2 auv;
void main()
{
    gl_Position = ortho*vec4(aPos,0.0, 1.0);
    ourColor = aColor;	
    pos =aPos;
    auv = uv;	
}
