#version 330

layout(location=0) in vec2 v_position;

// camera parameters for transforming the object position
// and scaling the subtex to the correct size
layout (std140) uniform camera {
    // view matrix (world to view space)
    mat4  view;
    // projection matrix (view to clip space)
    mat4  proj;
    // inverse zoom factor (1.0 / zoom)
    float inv_zoom;
    // inverse viewport size (1.0 / viewport size)
    vec2  inv_viewport_size;
};

// position of the object in world space
uniform vec3 obj_world_position;

// parameters for scaling and moving the subtex
// to the correct position in clip space

// animation scalefactor
// scales the vertex positions so that they
// match the subtex dimensions
//
// high animation scale = downscale subtex
// low animation scale = upscale subtex
uniform float scale;

// size of the frame (in pixels)
uniform vec2 frame_size;

void main() {
    // translate the position of the object from world space to clip space
    // this is the position where we want to draw the subtex in 2D
	vec4 obj_clip_pos = proj * view * vec4(obj_world_position, 1.0);

    // subtex has to be scaled to account for the zoom factor
    // and the animation scale factor. essentially this is (animation scale / zoom).
    float zoom_scale = scale * inv_zoom;

    // Scale the subtex vertices
    // we have to account for the viewport size to get the correct dimensions
    // and then scale the frame to the zoom factor to get the correct size
    vec2 vert_scale = zoom_scale * frame_size * inv_viewport_size;

    // create a move matrix for positioning the vertices
    // uses the vert scale and the transformed object position in clip space
    mat4 move = mat4(vert_scale.x,   0.0,            0.0,            0.0,
                     0.0,            vert_scale.y,   0.0,            0.0,
                     0.0,            0.0,            1.0,            0.0,
                     obj_clip_pos.x, obj_clip_pos.y, obj_clip_pos.z, 1.0);

    // calculate the final vertex position
    gl_Position = move * vec4(v_position, 0.0, 1.0);
}
