#version 330

in float v_cost;

out vec4 out_col;

void main()
{
    if (v_cost == 255.0) {
        out_col = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }
    float cost = (v_cost / 256) * 2.0;
    float red = clamp(cost, 0.0, 1.0);
    float green = clamp(2.0 - cost, 0.0, 1.0);
	out_col = vec4(red, green, 0.0, 1.0);
}
