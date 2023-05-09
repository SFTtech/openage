#version 330

layout(location=0) in vec2 v_position;
layout(location=1) in vec2 uv;

out vec2 vert_uv;

// transformation for object (not vertex!) position to clip space
uniform mat4 view;
uniform mat4 proj;

// can be used to move the object position in world space _before_
// it's transformed to clip space
// this is usually unnecessary because we want to draw the
// subtex where the object is, so this can be set to the identity matrix
uniform mat4 model;

// position of the object in world space
uniform vec3 obj_world_position;

uniform bool flip_x;
uniform bool flip_y;

// parameters for scaling and moving the subtex
// to the correct position in clip space

// offset from the subtex anchor
// moves the subtex relative to the subtex center
uniform vec2 anchor_offset;

// scales the vertex positions so that they
// match the subtex dimensions
uniform vec2 scale;

void main() {
    // translate the position of the object from world space to clip space
    // this is the position where we want to draw the subtex in 2D
	vec4 obj_clip_pos = proj * view * model * vec4(obj_world_position, 1.0);

    // offset the clip position by the offset of the subtex anchor
    // this basically aligns the object position and the subtex anchor
    obj_clip_pos += vec4(anchor_offset.xy, 0.0, 0.0);

    // create a move matrix for positioning the vertices
    // uses the scale and the transformed object position in clip space
    mat4 move = mat4(scale.x, 0.0, 0.0, 0.0,
                     0.0, scale.y, 0.0, 0.0,
                     0.0, 0.0, 1.0, 0.0,
                     obj_clip_pos.x, obj_clip_pos.y, obj_clip_pos.z, 1.0);

    // finally calculate the vertex position
    gl_Position = move * vec4(v_position, 0.0, 1.0);
    float uv_x = float(!flip_x) * uv.x + float(flip_x) * (1.0 - uv.x);
    float uv_y = float(flip_y) * uv.y + float(!flip_y) * (1.0 - uv.y);
    vert_uv = vec2(uv_x, uv_y);
}
