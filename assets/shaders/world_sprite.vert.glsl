#version 330

layout(location=0) in vec2 v_position;
layout(location=1) in vec2 uv;

out vec2 vert_uv;

// transformation for getting the position
// of the sprite in screen space
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

// position of the object in world space
uniform vec3 obj_world_position;

// parameters for scaling and moving the subtex
// to the correct position in screen space

// offset from the subtex anchor
// moves the subtex relative to the subtex center
uniform vec3 anchor_offset;

// scales the width and height of the subtex
uniform vec3 scale;

void main() {
    // translate the position of the object from world space to screen space
    // this is the position where we want to draw the subtex in 2D
	vec4 obj_screen_position = proj * view * model * vec4(obj_world_position, 1.0);

    // offset the screen position by the offset of the subtex anchor
    // this basically aligns the object position and the subtex anchor
    obj_screen_position += vec4(anchor_offset.xyz, 0.0);

    // create a move matrix that contains the scale and the
    // anchored screen position of the object
    mat4 move = mat4(scale.x, 0.0, 0.0, 0.0,
                     0.0, scale.y, 0.0, 0.0,
                     0.0, 0.0, 1.0, 0.0,
                     obj_screen_position.x, obj_screen_position.y, obj_screen_position.z, 1.0);

    // finally calculate the vertex position
    gl_Position = move * vec4(v_position, 0.0, 1.0);
    vert_uv = vec2(uv.x, 1.0 - uv.y);
}
