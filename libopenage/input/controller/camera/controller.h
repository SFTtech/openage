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
	Controller();

	~Controller() = default;

	/**
	 * Process an input event from the input manager.
	 *
	 * @param ev Input event and arguments.
	 * @param ctx Binding context that maps input events to camera actions.
	 *
	 * @return true if the event is accepted, else false.
	 */
	bool process(const event_arguments &ev_args, const std::shared_ptr<BindingContext> &ctx);
};

/**
 * Setup default camera action bindings:
 *
 * - Keyboard arrows: move camera.
 * - Mouse movement to window edge: move camera.
 * - Mouse wheel: zoom camera.
 *
 * TODO: Make this configurable.
 *
 * @param ctx Binding context the actions are added to.
 * @param cam Controlled camera.
 * @param cam_manager Camera manager for persistent movements.
 */
void setup_defaults(const std::shared_ptr<BindingContext> &ctx,
                    const std::shared_ptr<renderer::camera::Camera> &cam,
                    const std::shared_ptr<renderer::camera::CameraManager> &cam_manager);

} // namespace input::camera
} // namespace openage
