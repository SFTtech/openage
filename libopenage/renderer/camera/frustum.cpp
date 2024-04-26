// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "frustum.h"

#include <array>
#include <numbers>
#include <string>
#include <utility>

#include "coord/pixel.h"
#include "coord/scene.h"
#include "renderer/renderer.h"
#include "renderer/resources/buffer_info.h"

namespace openage::renderer::camera {

/*
    Eigen::Vector3f top_face_normal;
    float top_face_distance;

    Eigen::Vector3f bottom_face_normal;
    float bottom_face_distance;

    Eigen::Vector3f right_face_normal;
    float right_face_distance;

    Eigen::Vector3f left_face_normal;
    float left_face_distance;

    Eigen::Vector3f far_face_normal;
    float far_face_distance;

    Eigen::Vector3f near_face_normal;
    float near_face_distance;
*/

Frustum::Frustum() : 
    top_face_normal{Eigen::Vector3f(0.0f, 0.0f, 0.0f)},
    top_face_distance{0.0f},
    bottom_face_normal{Eigen::Vector3f(0.0f, 0.0f, 0.0f)},
    bottom_face_distance{0.0f},
    right_face_normal{Eigen::Vector3f(0.0f, 0.0f, 0.0f)},
    right_face_distance{0.0f},
    left_face_normal{Eigen::Vector3f(0.0f, 0.0f, 0.0f)},
    left_face_distance{0.0f},
    far_face_normal{Eigen::Vector3f(0.0f, 0.0f, 0.0f)},
    far_face_distance{0.0f},
    near_face_normal{Eigen::Vector3f(0.0f, 0.0f, 0.0f)},
    near_face_distance{0.0f}
{
}

void Frustum::Recalculate(util::Vector2s &viewport_size, float near_distance, float far_distance, Eigen::Vector3f &scene_pos, Eigen::Vector3f cam_direction, Eigen::Vector3f up_direction, float real_zoom) {
    // offsets are adjusted by zoom
	// this is the same calculation as for the projection matrix
	float halfscreenwidth = viewport_size[0] / 2;
	float halfscreenheight = viewport_size[1] / 2;

    float halfwidth = halfscreenwidth * real_zoom;
	float halfheight = halfscreenheight * real_zoom;

    Eigen::Vector3f direction = cam_direction.normalized();
    Eigen::Vector3f eye = scene_pos;
	Eigen::Vector3f center = scene_pos + direction;

    // calculate up (u) and right (s) vectors for camera
	// these define the camera plane in 3D space that the input
    Eigen::Vector3f f = center - eye;
	f.normalize();
	Eigen::Vector3f s = f.cross(up_direction);
	s.normalize();
	Eigen::Vector3f u = s.cross(f);
	u.normalize();

    Eigen::Vector3f near_position = scene_pos - direction * near_distance;
    Eigen::Vector3f far_position = scene_pos + direction * far_distance;

    Eigen::Vector3f near_top_left = near_position - s * halfwidth + u * halfheight;
    Eigen::Vector3f near_top_right = near_position + s * halfwidth + u * halfheight;
    Eigen::Vector3f near_bottom_left = near_position - s * halfwidth - u * halfheight;
    Eigen::Vector3f near_bottom_right = near_position + s * halfwidth - u * halfheight;

    Eigen::Vector3f far_top_left = far_position - s * halfwidth + u * halfheight;
    Eigen::Vector3f far_top_right = far_position + s * halfwidth + u * halfheight;
    Eigen::Vector3f far_bottom_left = far_position - s * halfwidth - u * halfheight;
    Eigen::Vector3f far_bottom_right = far_position + s * halfwidth - u * halfheight;

    this->near_face_normal = -1.0f * cam_direction.normalized();
    this->far_face_normal = cam_direction.normalized();

    this->near_face_distance = this->near_face_normal.dot(near_position) * -1.0f;
    this->far_face_distance = this->far_face_normal.dot(far_position) * -1.0f;

    this->left_face_normal = (near_bottom_left - near_top_left).cross(far_bottom_left - near_bottom_left);
    this->right_face_normal = (far_bottom_right - near_bottom_right).cross(near_bottom_right - near_top_right);
    this->top_face_normal = (near_top_right - near_top_left).cross(near_top_left - far_top_left);
    this->bottom_face_normal = (near_bottom_left - far_bottom_left).cross(near_bottom_right - near_bottom_left);

    log::log(INFO << "Compute near and far points");
    log::log(INFO << "Near Top Left: " << near_top_left[0] << ", " << near_top_left[1] << ", " << near_top_left[2]);
    log::log(INFO << "Near Top Right: " << near_top_right[0] << ", " << near_top_right[1] << ", " << near_top_right[2]);
    log::log(INFO << "Near Bottom Left: " << near_bottom_left[0] << ", " << near_bottom_left[1] << ", " << near_bottom_left[2]);
    log::log(INFO << "Near Bottom Right: " << near_bottom_right[0] << ", " << near_bottom_right[1] << ", " << near_bottom_right[2]);
    log::log(INFO << "Far Top Left: " << far_top_left[0] << ", " << far_top_left[1] << ", " << far_top_left[2]);
    log::log(INFO << "Far Top Right: " << far_top_right[0] << ", " << far_top_right[1] << ", " << far_top_right[2]);
    log::log(INFO << "Far Bottom Left: " << far_bottom_left[0] << ", " << far_bottom_left[1] << ", " << far_bottom_left[2]);
    log::log(INFO << "Far Bottom Right: " << far_bottom_right[0] << ", " << far_bottom_right[1] << ", " << far_bottom_right[2]);

    this->left_face_normal.normalize();
    this->right_face_normal.normalize();
    this->top_face_normal.normalize();
    this->bottom_face_normal.normalize();

    this->left_face_distance = 1.f * this->left_face_normal.dot(near_bottom_left);
    this->right_face_distance = 1.f * this->right_face_normal.dot(far_bottom_right);
    this->top_face_distance = 1.f * this->top_face_normal.dot(near_top_right);
    this->bottom_face_distance = 1.f * this->bottom_face_normal.dot(near_bottom_left);

    log::log(INFO << "Computed Frustum with planes \n"
	              << "Top: <" << this->top_face_normal[0] << ", " << this->top_face_normal[1] << ", " << this->top_face_normal[2] << ">, " << this->top_face_distance << "\n"
	              << "Bottom: <" << this->bottom_face_normal[0] << ", " << this->bottom_face_normal[1] << ", " << this->bottom_face_normal[2] << ">, " << this->bottom_face_distance << "\n"
	              << "Left: <" << this->left_face_normal[0] << ", " << this->left_face_normal[1] << ", " << this->left_face_normal[2] << ">, " << this->left_face_distance << "\n"
	              << "Right: <" << this->right_face_normal[0] << ", " << this->right_face_normal[1] << ", " << this->right_face_normal[2] << ">, " << this->right_face_distance << "\n"
	              << "Far: <" << this->far_face_normal[0] << ", " << this->far_face_normal[1] << ", " << this->far_face_normal[2] << ">, " << this->far_face_distance << "\n"
	              << "Near: <" << this->near_face_normal[0] << ", " << this->near_face_normal[1] << ", " << this->near_face_normal[2] << ">, " << this->near_face_distance << "\n"
    );
}

bool Frustum::is_in_frustum(Eigen::Vector3f& pos) const {
    float distance;

    distance = this->top_face_normal.dot(pos) - this->top_face_distance;
    if (distance < 0) {
        return false;
    }

    distance = this->bottom_face_normal.dot(pos) - this->bottom_face_distance;
    if (distance < 0) {
        return false;
    }

    distance = this->left_face_normal.dot(pos) - this->left_face_distance;
    if (distance < 0) {
        return false;
    }

    distance = this->right_face_normal.dot(pos) - this->right_face_distance;
    if (distance < 0) {
        return false;
    }

    distance = this->far_face_normal.dot(pos) - this->far_face_distance;
    if (distance < 0) {
        return false;
    }

    distance = this->bottom_face_normal.dot(pos) - this->bottom_face_distance;
    if (distance < 0) {
        return false;
    }

    return true;
}

}