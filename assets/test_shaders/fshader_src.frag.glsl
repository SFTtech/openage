#version 330

in vec2 v_uv;
uniform sampler2D tex;
uniform uint u_id;

layout(location=0) out vec4 col;
layout(location=1) out uint id;

void main() {
	vec4 tex_val = texture(tex, v_uv);
	if (tex_val.a == 0) {
		discard;
	}
	col = tex_val;
	id = u_id + 1u;
}
