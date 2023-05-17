#version 330

in vec2 v_uv;
uniform sampler2D tex;
uniform uint u_id;

layout(location=0) out vec4 col;
layout(location=1) out uint id;

void main() {
	vec4 tex_val = texture(tex, v_uv);
	int alpha = int(round(tex_val.a * 255));
	switch (alpha) {
		case 0:
		discard;
		break;
		case 254:
		col = vec4(1.0f, 0.0f, 0.0f, 1.0f);
		break;
		case 252:
		col = vec4(0.0f, 1.0f, 0.0f, 1.0f);
		break;
		case 250:
		col = vec4(0.0f, 0.0f, 1.0f, 1.0f);
		break;
		default:
	    col = tex_val;
		break;
	}
	id = u_id;
}
