// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>
#include <unordered_map>

#include "input/event.h"

namespace openage::input {

using action_flags_t = std::unordered_map<std::string, std::string>;
using action_func_t = std::function<void(const event_arguments &args)>;


/**
 * Action types.
 *
 * All action types (except \p input_action_t::CUSTOM) have a default
 * action.
 */
enum class input_action_t {
	PUSH_CONTEXT,
	POP_CONTEXT,
	REMOVE_CONTEXT,
	GAME,
	CAMERA,
	HUD,
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
	const input_action_t action_type;
	const std::optional<action_func_t> action = std::nullopt;
	const action_flags_t flags = {};
};


} // namespace openage::input
