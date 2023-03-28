// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "manager.h"

#include <eigen3/Eigen/Dense>

#include "renderer/camera/camera.h"

namespace openage::renderer::camera {

CameraManager::CameraManager(const std::shared_ptr<renderer::camera::Camera> &camera) :
	camera{camera},
	move_directions{static_cast<int>(MoveDirection::NONE)},
	zoom_direction{static_cast<int>(ZoomDirection::NONE)},
	move_speed{0.2f},
	zoom_speed{0.05f} {
}

void CameraManager::update() {
	if (this->move_directions != static_cast<int>(MoveDirection::NONE)) {
		Eigen::Vector3f move_dir{0.0f, 0.0f, 0.0f};

		if (this->move_directions & static_cast<int>(MoveDirection::LEFT)) {
			move_dir += Eigen::Vector3f(1.0f, 0.0f, -1.0f);
		}
		if (this->move_directions & static_cast<int>(MoveDirection::RIGHT)) {
			move_dir += Eigen::Vector3f(-1.0f, 0.0f, 1.0f);
		}
		if (this->move_directions & static_cast<int>(MoveDirection::TOP)) {
			move_dir += Eigen::Vector3f(1.0f, 0.0f, 1.0f);
		}
		if (this->move_directions & static_cast<int>(MoveDirection::BOTTOM)) {
			move_dir += Eigen::Vector3f(-1.0f, 0.0f, -1.0f);
		}

		this->camera->move_rel(move_dir, this->move_speed);
	}

	if (this->zoom_direction != static_cast<int>(ZoomDirection::NONE)) {
		if (this->zoom_direction & static_cast<int>(ZoomDirection::IN)) {
			this->camera->zoom_in(this->zoom_speed);
		}
		else if (this->zoom_direction & static_cast<int>(ZoomDirection::OUT)) {
			this->camera->zoom_out(this->zoom_speed);
		}
	}
}

void CameraManager::set_move_directions(int directions) {
	this->move_directions = directions;
}

void CameraManager::set_zoom_direction(int direction) {
	this->zoom_direction = direction;
}

void CameraManager::set_move_speed(float speed) {
	this->move_speed = speed;
}

void CameraManager::set_zoom_speed(float speed) {
	this->zoom_speed = speed;
}

} // namespace openage::renderer::camera
