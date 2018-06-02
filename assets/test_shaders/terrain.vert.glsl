#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 uv;
layout (location = 2) in float terrain_index;
layout (location = 3) in float alpha_mask;

uniform mat4 ortho = mat4(1.0);
uniform mat4 dimet = mat4(1.0);  
/*mat4 ortho = mat4(2.0/1920.0,0.0,0.0,-1.0,
		0.0,-2.0/1080.0,0.0,1.0,
		0.0,0.0,1.0,0.0,
		0.0,0.0,1.0,1.0);
*/
out vec2 pos;
out vec2 auv;
out float tex_ind;
void main()
{
    gl_Position = ortho*dimet*vec4(aPos,0.0, 1.0);
    pos =aPos;
    auv = uv;
    tex_ind = terrain_index;
}
