// Copyright 2022-2022 the openage authors. See copying.md for legal info.

#include "camera.h"

namespace openage::renderer::camera {

Camera::Camera(util::Vector2s viewport_size) :
	scene_pos{Eigen::Vector3f(0.0f, 10.0f, 0.0f)},
	viewport_size{viewport_size},
	zoom{0.025f} {
	this->look_at_scene(Eigen::Vector3f(0.0f, 0.0f, 0.0f));
}

Camera::Camera(util::Vector2s viewport_size,
               Eigen::Vector3f scene_pos,
               float zoom) :
	scene_pos{scene_pos},
	viewport_size{viewport_size},
	zoom{zoom} {
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

	this->move(new_pos);
}

void Camera::look_at_coord(util::Vector3f coord_pos) {
	// convert coord pos to scene pos
	auto scene_pos = Eigen::Vector3f(
		-coord_pos[1],
		coord_pos[2],
		coord_pos[0]);
	this->look_at_scene(scene_pos);
}

void Camera::move(Eigen::Vector3f scene_pos) {
	// TODO: Check and set bounds for where the camera can go and check them here

	this->scene_pos = scene_pos;
}

void Camera::move_rel(Eigen::Vector3f direction, float delta) {
	this->move(this->scene_pos + (direction * delta));
}

void Camera::set_zoom(float zoom) {
	if (zoom < MAX_ZOOM_IN) {
		zoom = MAX_ZOOM_IN;
	}

	this->zoom = zoom;
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

Eigen::Matrix4f Camera::get_view_matrix() {
	// Fixed angle
	// yaw   = -135 degrees
	// pitch = 30 degrees
	float direction_x = -1 * (sqrt(6) / 4);
	float direction_y = 0.5f;
	float direction_z = -1 * (sqrt(6) / 4);

	Eigen::Vector3f direction{
		direction_x,
		direction_y,
		direction_z};
	direction.normalize();

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

	return mat;
}

Eigen::Matrix4f Camera::get_projection_matrix() {
	// get center of viewport as the focus point
	float halfwidth = viewport_size[0] / 2;
	float halfheight = viewport_size[1] / 2;

	// zoom by narrowing or widening viewport around focus point.
	// narrow viewport => zoom in (projected area gets *smaller* while screen size stays the same)
	// widen viewport => zoom out (projected area gets *bigger* while screen size stays the same)
	float left = -(this->zoom * halfwidth);
	float right = this->zoom * halfwidth;
	float bottom = -(this->zoom * halfheight);
	float top = this->zoom * halfheight;

	Eigen::Matrix4f mat = Eigen::Matrix4f::Identity();
	mat(0, 0) = 2.0f / (right - left);
	mat(1, 1) = 2.0f / (top - bottom);
	mat(2, 2) = -2.0f / (1000.0f - (-1.0f)); // clip near and far planes (TODO: necessary?)
	mat(0, 3) = -(right + left) / (right - left);
	mat(1, 3) = -(top + bottom) / (top - bottom);
	mat(2, 3) = -(1000.0f + (-1.0f)) / (1000.0f - (-1.0f)); // clip near and far planes (TODO: necessary?)

	return mat;
}

} // namespace openage::renderer::camera
