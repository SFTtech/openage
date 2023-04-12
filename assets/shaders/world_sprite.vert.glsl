#version 330

layout(location=0) in vec2 v_position;
layout(location=1) in vec2 uv;

out vec2 vert_uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform vec3 obj_world_position;
uniform vec3 anchor_offset;
uniform vec3 scale;

void main() {
	vec4 obj_screen_position = proj * view * model * vec4(obj_world_position, 1.0);
    obj_screen_position += vec4(anchor_offset.xyz, 0.0);
    mat4 move = mat4(scale.x, 0.0, 0.0, 0.0,
                     0.0, scale.y, 0.0, 0.0,
                     0.0, 0.0, 1.0, 0.0,
                     obj_screen_position.x, obj_screen_position.y, obj_screen_position.z, 1.0);
    gl_Position = move * vec4(v_position, 0.0, 1.0);
    vert_uv = vec2(uv.x, 1.0 - uv.y);
}
