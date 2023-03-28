// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include "input/event.h"

namespace openage::input::camera {

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
     *
     * @return true if the event is accepted, else false.
     */
	bool process(const event_arguments &ev_args, const std::shared_ptr<BindingContext> &ctx);
};

} // namespace openage::input::camera
