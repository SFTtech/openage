#version 330

in float v_cost;

out vec4 outcol;

void main() {
    int cost = int(v_cost);
    if (!bool(cost & 0x10)) {
        // not pathable
        outcol = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    if (bool(cost & 0x20)) {
        // line of sight
        outcol = vec4(1.0, 1.0, 1.0, 1.0);
        return;
    }

    outcol = vec4(0.7, 0.7, 0.7, 1.0);
}
