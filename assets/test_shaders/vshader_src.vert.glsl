
#version 330

layout(location=0) in vec2 position;
layout(location=1) in vec2 uv;
uniform mat4 mvp;
uniform vec2 pos = vec2(0.0,0.0);
out vec2 v_uv;

void main() {
	gl_Position = mvp * vec4(position + pos, 0.0, 1.0);

  v_uv = vec2(uv.x, uv.y);
}

