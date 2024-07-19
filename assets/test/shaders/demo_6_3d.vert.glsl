#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

out vec2 tex_pos;

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

void main() {
	gl_Position = proj * view * vec4(position, 1.0);
    tex_pos = vec2(uv.x, 1.0 - uv.y);
}
