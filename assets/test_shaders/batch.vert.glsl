#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 auv;
layout (location = 3) in float active_id;
uniform mat4 ortho = mat4(1.0);

out vec4 ourColor;
out vec2 pos;
out vec2 uv;
out float a_id;

void main()
{
    gl_Position = ortho*vec4(aPos,0.0, 1.0);
    ourColor = aColor;
    pos =aPos;
    uv = auv;	
    a_id = active_id;
}
