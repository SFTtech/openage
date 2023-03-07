// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>
#include <stack>
#include <unordered_map>
#include <vector>

#include "input/event.h"

namespace openage::input {

using action_func_t = std::function<void(const Event &e)>;

/**
 * An input context contains all keybindings and actions
 * active in e.g. the HUD only.
 * That way, each context can have the same keys
 * assigned to different actions, the active context
 * decides, which one to trigger.
 */
class InputContext {
public:
	/**
	 * Create an input context.
	 */
	InputContext();

	virtual ~InputContext() = default;

	/**
	 * bind a specific event
	 * this is the second matching priority
	 */
	void bind(const Event &ev, const action_func_t act);

private:
	/**
	 * map specific overriding events
	 */
	std::unordered_map<Event, action_func_t> by_event;
};

} // namespace openage::input
