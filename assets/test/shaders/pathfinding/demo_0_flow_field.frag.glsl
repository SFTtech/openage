#version 330

/// Flow field value
in float flow_val;

/// Integration field flags
in float int_val;

out vec4 outcol;

int WAVEFRONT_BLOCKED = 0x04;
int LINE_OF_SIGHT = 0x20;
int PATHABLE = 0x10;

void main() {
    int flow_flags = int(flow_val) & 0xF0;
    int int_flags = int(int_val);
    if (bool(int_flags & WAVEFRONT_BLOCKED)) {
        // wavefront blocked
        outcol = vec4(0.9, 0.9, 0.9, 1.0);
        return;
    }

    if (bool(int_flags & LINE_OF_SIGHT)) {
        // line of sight
        outcol = vec4(1.0, 1.0, 1.0, 1.0);
        return;
    }

    if (bool(flow_flags & PATHABLE)) {
        // pathable
        outcol = vec4(0.7, 0.7, 0.7, 1.0);
        return;
    }

    // not pathable
    outcol = vec4(0.0, 0.0, 0.0, 1.0);
}
