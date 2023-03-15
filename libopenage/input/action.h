// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>
#include <unordered_map>

#include "input/event.h"

namespace openage::input {


using action_func_t = std::function<void(const event_arguments &args)>;


/**
 * Action types.
 *
 * All action types (except \p action_t::CUSTOM) have a default
 * action.
 */
enum class action_t {
	PUSH_CONTEXT,
	POP_CONTEXT,
	REMOVE_CONTEXT,
	CONTROLLER,
	GUI,
	CUSTOM,
};

/**
 * Action taken by the input manager when receiving an input.
 *
 * @param action_type Action type. May determine a default action if \p action is not defined.
 * @param action Executes custom code for the action based on the received event (optional).
 * @param flags Additional parameters for the (default) action.
 */
struct input_action {
	action_t action_type;
	std::optional<action_func_t> action;
	std::unordered_map<std::string, std::string> flags = {};
};


} // namespace openage::input
