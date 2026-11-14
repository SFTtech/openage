#version 330

in vec2 v_uv;
uniform sampler2D tex;
uniform uint u_id;

layout(location=0) out vec4 col;
layout(location=1) out uint id;

void main() {
	vec4 tex_val = texture(tex, v_uv);
	int alpha = int(round(tex_val.a * 255));

	if (alpha == 0) {
	    discard;
	} else if (alpha == 254) {
	    col = vec4(1.0, 0.0, 0.0, 1.0);
	} else if (alpha == 252) {
	    col = vec4(0.0, 1.0, 0.0, 1.0);
	} else if (alpha == 250) {
	    col = vec4(0.0, 0.0, 1.0, 1.0);
	} else {
	    col = tex_val;
	}

	id = u_id;
}
