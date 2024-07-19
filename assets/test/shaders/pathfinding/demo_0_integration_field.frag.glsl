#version 330

in float v_cost;

out vec4 out_col;

void main()
{
    if (v_cost > 512.0) {
        out_col = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }
    float cost = 0.05 * v_cost;
    float green = clamp(1.0 - cost, 0.0, 1.0);
	out_col = vec4(0.75, green, 0.5, 1.0);
}
