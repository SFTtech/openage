// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "frustum_3d.h"


namespace openage::renderer::camera {

Frustum3d::Frustum3d(const util::Vector2s &viewport_size,
                     const float near_distance,
                     const float far_distance,
                     const Eigen::Vector3f &camera_pos,
                     const Eigen::Vector3f &cam_direction,
                     const Eigen::Vector3f &up_direction,
                     const float zoom_factor) {
	this->update(viewport_size,
	             near_distance,
	             far_distance,
	             camera_pos,
	             cam_direction,
	             up_direction,
	             zoom_factor);
}

void Frustum3d::update(const util::Vector2s &viewport_size,
                       const float near_distance,
                       const float far_distance,
                       const Eigen::Vector3f &camera_pos,
                       const Eigen::Vector3f &cam_direction,
                       const Eigen::Vector3f &up_direction,
                       const float zoom_factor) {
	// offsets are adjusted by zoom
	// this is the same calculation as for the projection matrix
	float halfscreenwidth = viewport_size[0] / 2;
	float halfscreenheight = viewport_size[1] / 2;

	float halfwidth = halfscreenwidth * zoom_factor;
	float halfheight = halfscreenheight * zoom_factor;

	Eigen::Vector3f direction = cam_direction.normalized();
	Eigen::Vector3f eye = camera_pos;
	Eigen::Vector3f center = camera_pos + direction;

	// calculate up (u) and right (s) vectors for camera
	// these define the camera plane in 3D space that the input
	Eigen::Vector3f f = center - eye;
	f.normalize();
	Eigen::Vector3f s = f.cross(up_direction);
	s.normalize();
	Eigen::Vector3f u = s.cross(f);
	u.normalize();

	// calculate distance of the camera position to the near and far plane
	Eigen::Vector3f near_position = camera_pos + direction * near_distance;
	Eigen::Vector3f far_position = camera_pos + direction * far_distance;

	// The frustum is a cuboid box with 8 points defining it (4 on the near plane, 4 on the far plane)
	Eigen::Vector3f near_top_left = near_position - s * halfwidth + u * halfheight;
	Eigen::Vector3f near_top_right = near_position + s * halfwidth + u * halfheight;
	Eigen::Vector3f near_bottom_left = near_position - s * halfwidth - u * halfheight;
	Eigen::Vector3f near_bottom_right = near_position + s * halfwidth - u * halfheight;

	Eigen::Vector3f far_top_left = far_position - s * halfwidth + u * halfheight;
	// Eigen::Vector3f far_top_right = far_position + s * halfwidth + u * halfheight; // unused
	Eigen::Vector3f far_bottom_left = far_position - s * halfwidth - u * halfheight;
	Eigen::Vector3f far_bottom_right = far_position + s * halfwidth - u * halfheight;

	// The near and far planes are easiest to compute, as they should be in the direction of the camera
	this->near_face_normal = cam_direction.normalized();
	this->far_face_normal = -1.0f * cam_direction.normalized();

	// The distance of the plane from the origin is the dot product of the normal and a point on the plane
	this->near_face_distance = this->near_face_normal.dot(near_position);
	this->far_face_distance = this->far_face_normal.dot(far_position);

	// Each left, right, top, and bottom plane are defined by three points on the plane
	this->left_face_normal = (near_bottom_left - near_top_left).cross(far_bottom_left - near_bottom_left);
	this->right_face_normal = (far_bottom_right - near_bottom_right).cross(near_bottom_right - near_top_right);
	this->top_face_normal = (near_top_right - near_top_left).cross(near_top_left - far_top_left);
	this->bottom_face_normal = (near_bottom_left - far_bottom_left).cross(near_bottom_right - near_bottom_left);

	// for orthographic projection, the normal of left/right should equal -s/s
	// and the normal of top/bottom should equal u/-u
	this->left_face_normal.normalize();
	this->right_face_normal.normalize();
	this->top_face_normal.normalize();
	this->bottom_face_normal.normalize();

	// calculate the distance of the planes to the origin
	this->left_face_distance = this->left_face_normal.dot(near_bottom_left);
	this->right_face_distance = this->right_face_normal.dot(far_bottom_right);
	this->top_face_distance = this->top_face_normal.dot(near_top_right);
	this->bottom_face_distance = this->bottom_face_normal.dot(near_bottom_left);
}

bool Frustum3d::in_frustum(const Eigen::Vector3f &pos) const {
	// For each plane, if a point is behind one of the frustum planes, it is not within the frustum
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

} // namespace openage::renderer::camera
