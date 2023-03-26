// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "camera.h"

namespace openage::input {

CameraController::CameraController(const std::shared_ptr<renderer::camera::Camera> &camera) :
	camera{camera} {}

bool CameraController::process(const event_arguments &ev) {
	return false;
}

} // namespace openage::input
