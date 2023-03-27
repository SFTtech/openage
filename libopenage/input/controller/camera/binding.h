// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>
#include <unordered_map>

#include "event/event.h"
#include "input/event.h"

namespace openage::input::camera {

using binding_flags_t = std::unordered_map<std::string, std::string>;
using binding_func_t = std::function<void(const event_arguments &e)>;


/**
 * Action taken by the controller when receiving an input.
 *
 * @param action Maps an input event to a camera action.
 * @param flags Additional parameters for the transformation.
 */
struct binding_action {
	const binding_func_t action;
	const binding_flags_t flags = {};
};

} // namespace openage::input::camera
