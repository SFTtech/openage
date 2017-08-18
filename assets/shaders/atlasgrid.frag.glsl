#version 120

uniform lowp float qt_Opacity;
uniform lowp sampler2D tex;

// Total number of columns in a square atlas grid.
uniform highp float columns;

// Spacing in the atlas.
uniform highp float spacing;

varying vec4 v_color;

// ID in a square atlas grid with number of columns 'columns'.
varying float v_cell_id;

void main() {
	vec2 border = vec2(spacing);
	vec2 in_grid_coord = vec2(mod(v_cell_id, columns), floor(v_cell_id / columns)) + border;
	vec2 subtex_coord = (in_grid_coord + gl_PointCoord * (vec2(1.0) - border)) / columns;

	if (texture2D(tex, subtex_coord).a < 0.1)
		discard;
	else;
		gl_FragColor = vec4(v_color.rgb, v_color.a * texture2D(tex, subtex_coord).a) * qt_Opacity;
}
