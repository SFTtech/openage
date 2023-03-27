// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include "input/event.h"

namespace openage {

namespace renderer::camera {
class Camera;
class CameraManager;
} // namespace renderer::camera

namespace input::camera {

class BindingContext;

/**
 * Control a camera with input from the input manager.
 */
class Controller {
public:
	Controller(const std::shared_ptr<renderer::camera::Camera> &camera,
	           const std::shared_ptr<renderer::camera::CameraManager> &manager);

	~Controller() = default;

	/**
     * Process an input event from the input manager.
     *
     * @param ev Input event and arguments.
     *
     * @return true if the event is accepted, else false.
     */
	bool process(const event_arguments &ev_args, const std::shared_ptr<BindingContext> &ctx);

private:
	/**
     * Camera to control.
     */
	std::shared_ptr<renderer::camera::Camera> camera;

	/**
     * Camera manager for persistent movements/zooms.
     */
	std::shared_ptr<renderer::camera::CameraManager> manager;
};

} // namespace input::camera
} // namespace openage
