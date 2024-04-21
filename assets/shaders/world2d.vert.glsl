#version 330

layout(location=0) in vec2 v_position;
layout(location=1) in vec2 uv;

out vec2 vert_uv;

// camera parameters for transforming the object position
// and scaling the subtex to the correct size
layout (std140) uniform camera {
    // view matrix (world to view space)
    mat4  view;
    // projection matrix (view to clip space)
    mat4  proj;
    // inverse zoom factor (1.0 / zoom)
    // high zoom = upscale subtex
    // low zoom = downscale subtex
    float inv_zoom;
    // inverse viewport size (1.0 / viewport size)
    vec2  inv_viewport_size;
};

// can be used to move the object position in world space _before_
// it's transformed to clip space
// this is usually unnecessary because we want to draw the
// subtex where the object is, so this can be set to the identity matrix
uniform mat4 model;

// position of the object in world space
uniform vec3 obj_world_position;

// flip the subtexture horizontally/vertically
uniform bool flip_x;
uniform bool flip_y;

// parameters for scaling and moving the subtex
// to the correct position in clip space

// animation scalefactor
// scales the vertex positions so that they
// match the subtex dimensions
//
// high animation scale = downscale subtex
// low animation scale = upscale subtex
uniform float scale;

// size of the subtex (in pixels)
uniform vec2 subtex_size;

// offset of the subtex anchor point
// from the subtex center (in pixels)
// used to move the subtex so that the anchor point
// is at the object position
uniform vec2 anchor_offset;

void main() {
    // translate the position of the object from world space to clip space
    // this is the position where we want to draw the subtex in 2D
	vec4 obj_clip_pos = proj * view * model * vec4(obj_world_position, 1.0);

    // subtex has to be scaled to account for the zoom factor
    // and the animation scale factor. essentially this is (animation scale / zoom).
    float zoom_scale = scale * inv_zoom;

    // Scale the subtex vertices
    // we have to account for the viewport size to get the correct dimensions
    // and then scale the subtex to the zoom factor to get the correct size
    vec2 vert_scale = zoom_scale * subtex_size * inv_viewport_size;

    // Scale the anchor offset with the same method as above
    // to get the correct anchor position in the viewport
    vec2 anchor_scale = zoom_scale * anchor_offset * inv_viewport_size;

    // if the subtex is flipped, we also need to flip the anchor offset
    // essentially, we invert the coordinates for the flipped axis
    float anchor_x = float(flip_x) * -1.0 * anchor_scale.x + float(!flip_x) * anchor_scale.x;
    float anchor_y = float(flip_y) * -1.0 * anchor_scale.y + float(!flip_y) * anchor_scale.y;

    // offset the clip position by the offset of the subtex anchor
    // imagine this as pinning the subtex to the object position at the subtex anchor point
    obj_clip_pos += vec4(anchor_x, anchor_y, 0.0, 0.0);

    // create a move matrix for positioning the vertices
    // uses the vert scale and the transformed object position in clip space
    mat4 move = mat4(vert_scale.x,   0.0,            0.0,            0.0,
                     0.0,            vert_scale.y,   0.0,            0.0,
                     0.0,            0.0,            1.0,            0.0,
                     obj_clip_pos.x, obj_clip_pos.y, obj_clip_pos.z, 1.0);

    // calculate the final vertex position
    gl_Position = move * vec4(v_position, 0.0, 1.0);

    // if the subtex is flipped, we also need to flip the uv tex coordinates
    // essentially, we invert the coordinates for the flipped axis

    // !flip_x is default because OpenGL uses bottom-left as its origin
    float uv_x = float(!flip_x) * uv.x + float(flip_x) * (1.0 - uv.x);
    float uv_y = float(flip_y) * uv.y + float(!flip_y) * (1.0 - uv.y);

    vert_uv = vec2(uv_x, uv_y);
}
