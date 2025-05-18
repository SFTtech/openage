#version 330

in vec2 vert_uv;

layout(location=0) out vec4 col;
layout(location=1) out uint id;

uniform sampler2D tex;
uniform uint u_id;

// position (top left corner) and size: (x, y, width, height)
uniform vec4 tile_params;

vec2 uv = vec2(
	vert_uv.x * tile_params.z + tile_params.x,
	vert_uv.y * tile_params.w + tile_params.y
);

void main() {
	vec4 tex_val = texture(tex, uv);
	int alpha = int(round(tex_val.a * 255));
	if (alpha == 0) {
		col = tex_val;
		discard;
		return;
	} else if (alpha == 254) {
		col = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	} else if (alpha == 252) {
		col = vec4(0.0f, 1.0f, 0.0f, 1.0f);
	} else if (alpha == 250) {
		col = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	} else {
		col = tex_val;
	}
	id = u_id;
}
