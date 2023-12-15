// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>
#include <memory>
#include <unordered_map>

#include "event/event.h"
#include "input/event.h"

namespace openage::input::game {

class Controller;

using binding_flags_t = std::unordered_map<std::string, std::string>;
using binding_func_t = std::function<const std::shared_ptr<event::Event>(const event_arguments &,
                                                                         const std::shared_ptr<Controller>)>;


/**
 * Action type for the event forwarding.
 *
 * Determines what is done with the created gamestate event
 *     - \p SEND forward the event (and all queued events) immediately
 *     - \p QUEUE queue the event
 *     - \p CLEAR clear the queue
 */
enum class forward_action_t {
	SEND,
	QUEUE,
	CLEAR,
};

/**
 * Action taken by the controller when receiving an input.
 *
 * @param action_type Event action type.
 * @param transform Maps an input event to a gamestate event.
 * @param flags Additional parameters for the transformation.
 */
struct binding_action {
	const forward_action_t action_type;
	const binding_func_t transform;
	const binding_flags_t flags = {};
};

} // namespace openage::input::game
