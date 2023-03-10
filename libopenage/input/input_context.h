// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>
#include <stack>
#include <unordered_map>
#include <vector>

#include "input/event.h"

namespace openage::input {

using action_func_t = std::function<void(const Event &e)>;

enum class action_t {
	PUSH_CONTEXT,
	POP_CONTEXT,
	REMOVE_CONTEXT,
	CONTROLLER,
	GUI,
};

struct InputAction {
	action_t type;
	action_func_t action;
};

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
	 * bind a specific key event to an action
     *
     * @param ev Input event triggering the action.
     * @param act Function executing the action.
	 */
	void bind(const KeyEvent &ev, const InputAction act);

	/**
     * Check whether a specific key event is bound in this context.
     *
     * @param ev Input event.
     *
     * @return true if event is bound, else false.
     */
	bool is_bound(const KeyEvent &ev) const;

	/**
     * Get the action(s) bound to a specific event.
     *
     * @param ev Input event triggering the action.
     */
	const InputAction &lookup(const KeyEvent &ev) const;

private:
	/**
	 * map specific overriding events
	 */
	std::unordered_map<KeyEvent, InputAction, event_hash> by_keyevent;
};

} // namespace openage::input
