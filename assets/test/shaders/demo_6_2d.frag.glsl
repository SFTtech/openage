#version 330

in vec2 vert_uv;

layout(location=0) out vec4 col;

uniform sampler2D tex;

// position (top left corner) and size: (x, y, width, height)
uniform vec4 tile_params;

vec2 uv = vec2(
	vert_uv.x * tile_params.z + tile_params.x,
	vert_uv.y * tile_params.w + tile_params.y
);

void main() {
	vec4 tex_val = texture(tex, uv);
	int alpha = int(round(tex_val.a * 255));
	switch (alpha) {
		case 0:
			col = tex_val;
			discard;
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
}
