// Copyright 2022-2023 the openage authors. See copying.md for legal info.

#include "camera.h"

namespace openage::renderer::camera {

Camera::Camera(util::Vector2s viewport_size) :
	scene_pos{Eigen::Vector3f(0.0f, 10.0f, 0.0f)},
	viewport_size{viewport_size},
	zoom{1.0f},
	max_zoom_out{64.0f},
	default_zoom_ratio{1.0f / 49},
	moved{true},
	zoom_changed{true},
	view{Eigen::Matrix4f::Identity()},
	proj{Eigen::Matrix4f::Identity()} {
	this->look_at_scene(Eigen::Vector3f(0.0f, 0.0f, 0.0f));
}

Camera::Camera(util::Vector2s viewport_size,
               Eigen::Vector3f scene_pos,
               float zoom,
               float max_zoom_out,
               float default_zoom_ratio) :
	scene_pos{scene_pos},
	viewport_size{viewport_size},
	zoom{zoom},
	max_zoom_out{max_zoom_out},
	default_zoom_ratio{default_zoom_ratio},
	moved{true},
	zoom_changed{true},
	view{Eigen::Matrix4f::Identity()},
	proj{Eigen::Matrix4f::Identity()} {
}

void Camera::look_at_scene(Eigen::Vector3f scene_pos) {
	if (scene_pos[1] > this->scene_pos[1]) {
		// TODO: camera can't look at a position that's
		//       higher than it's own position
	}

	// TODO: Although the below method should be faster, calculating and adding the direction
	//       vector from scene_pos to new_pos may be easier to understand
	//       i.e. new_pos = scene_pos + b/sin(30) * direction_vec

	// due to the fixed angle, the centered scene position
	// and the new camera position form a right triangle.
	//
	//           c - + new camera pos
	//          -    |b
	// center +------+
	//            a
	//
	// we can calculate the new camera position via the offset a
	// using the angle and length of side b.
	auto y_delta = this->scene_pos[1] - scene_pos[1]; // b (vertical distance)
	auto xz_distance = y_delta * sqrt(3); // a (horizontal distance); a = b * (cos(30°) / sin(30°))

	// get x and z offsets
	// the camera is pointed diagonally to the positive x and z axis
	// a is the length of the diagonal from camera.xz to scene_pos.xz
	// so the x and z offest are sides of a square with the same diagonal
	auto side_length = xz_distance / (sqrt(2));
	auto new_pos = Eigen::Vector3f(
		scene_pos[0] - side_length,
		this->scene_pos[1], // height unchanged
		scene_pos[2] - side_length);

	this->move_to(new_pos);
}

void Camera::look_at_coord(coord::scene3 coord_pos) {
	// convert coord pos to scene pos
	auto scene_pos = coord_pos.to_vector();
	this->look_at_scene(scene_pos);
}

void Camera::move_to(Eigen::Vector3f scene_pos) {
	// TODO: Check and set bounds for where the camera can go and check them here

	this->scene_pos = scene_pos;
	this->moved = true;
}

void Camera::move_rel(Eigen::Vector3f direction, float delta) {
	this->move_to(this->scene_pos + (direction * delta));
}

void Camera::set_zoom(float zoom) {
	if (zoom < Camera::MAX_ZOOM_IN) {
		zoom = Camera::MAX_ZOOM_IN;
	}
	else if (zoom > this->max_zoom_out) {
		zoom = this->max_zoom_out;
	}

	this->zoom = zoom;
	this->zoom_changed = true;
}

void Camera::zoom_in(float zoom_delta) {
	auto zoom = this->zoom - zoom_delta;
	this->set_zoom(zoom);
}

void Camera::zoom_out(float zoom_delta) {
	auto zoom = this->zoom + zoom_delta;
	this->set_zoom(zoom);
}

void Camera::resize(size_t width, size_t height) {
	this->viewport_size = util::Vector2s(width, height);
}

float Camera::get_zoom() const {
	return this->zoom;
}

const Eigen::Matrix4f &Camera::get_view_matrix() {
	if (not this->moved) {
		return this->view;
	}

	auto direction = cam_direction.normalized();

	Eigen::Vector3f eye = this->scene_pos;
	Eigen::Vector3f center = this->scene_pos - direction; // look in the direction of the camera
	Eigen::Vector3f up = Eigen::Vector3f(0.0f, 1.0f, 0.0f);

	Eigen::Vector3f f = center - eye;
	f.normalize();
	Eigen::Vector3f s = f.cross(up);
	s.normalize();
	Eigen::Vector3f u = s.cross(f);

	// View matrix
	// |  s[0]  s[1]  s[2] -dot(s,eye) |
	// |  u[0]  u[1]  u[2] -dot(u,eye) |
	// | -f[0] -f[1] -f[2]  dot(f,eye) |
	// |   0     0     0        1      |
	Eigen::Matrix4f mat = Eigen::Matrix4f::Identity();
	mat(0, 0) = s[0];
	mat(0, 1) = s[1];
	mat(0, 2) = s[2];
	mat(1, 0) = u[0];
	mat(1, 1) = u[1];
	mat(1, 2) = u[2];
	mat(2, 0) = -f[0];
	mat(2, 1) = -f[1];
	mat(2, 2) = -f[2];
	mat(0, 3) = -s.dot(eye);
	mat(1, 3) = -u.dot(eye);
	mat(2, 3) = f.dot(eye);

	// Cache matrix for subsequent calls
	this->view = mat;
	this->moved = false;

	return this->view;
}

const Eigen::Matrix4f &Camera::get_projection_matrix() {
	if (not this->zoom_changed) {
		return this->proj;
	}

	// get center of viewport as the focus point
	float halfwidth = viewport_size[0] / 2;
	float halfheight = viewport_size[1] / 2;

	// get zoom level
	float real_zoom = 0.5f * this->default_zoom_ratio * this->zoom;

	// zoom by narrowing or widening viewport around focus point.
	// narrow viewport => zoom in (projected area gets *smaller* while screen size stays the same)
	// widen viewport => zoom out (projected area gets *bigger* while screen size stays the same)
	float left = -(real_zoom * halfwidth);
	float right = real_zoom * halfwidth;
	float bottom = -(real_zoom * halfheight);
	float top = real_zoom * halfheight;

	Eigen::Matrix4f mat = Eigen::Matrix4f::Identity();
	mat(0, 0) = 2.0f / (right - left);
	mat(1, 1) = 2.0f / (top - bottom);
	mat(2, 2) = -2.0f / (1000.0f - (-1.0f)); // clip near and far planes (TODO: necessary?)
	mat(0, 3) = -(right + left) / (right - left);
	mat(1, 3) = -(top + bottom) / (top - bottom);
	mat(2, 3) = -(1000.0f + (-1.0f)) / (1000.0f - (-1.0f)); // clip near and far planes (TODO: necessary?)

	// Cache matrix for subsequent calls
	this->proj = mat;
	this->zoom_changed = false;

	return this->proj;
}

const util::Vector2s &Camera::get_viewport_size() const {
	return this->viewport_size;
}

} // namespace openage::renderer::camera
