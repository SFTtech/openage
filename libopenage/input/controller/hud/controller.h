// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include "input/event.h"


namespace openage::input::hud {
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
	 * @param ctx Binding context that maps input events to HUD actions.
	 *
	 * @return true if the event is accepted, else false.
	 */
	bool process(const event_arguments &ev_args, const std::shared_ptr<BindingContext> &ctx);
};

/**
 * Setup default HUD action bindings:
 *
 * - Mouse drag: selectION BOX
 *
 * TODO: Make this configurable.
 *
 * @param ctx Binding context the actions are added to.
 */
void setup_defaults(const std::shared_ptr<BindingContext> &ctx);

} // namespace openage::input::hud
