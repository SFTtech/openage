// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_KEYBINDS_KEYBIND_CONTEXT_H_
#define OPENAGE_KEYBINDS_KEYBIND_CONTEXT_H_

#include <functional>
#include <stack>
#include <unordered_map>
#include <vector>

#include "action.h"
#include "event.h"
#include "../handlers.h"

namespace openage {
namespace input {

class InputContext {

public:
	InputContext();

	/**
	 * a list of all keys which are bound in the current context
	 */
	std::vector<std::string> active_binds() const;

	/**
	 * bind a specific action idetifier
	 * this is the highest matching priority
	 */
	void bind(action_t type, const action_func_t act);

	/**
	 * bind a specific event
	 * this is the second matching priority
	 */
	void bind(const Event &ev, const action_func_t act);

	/**
	 * bind all events of a specific class
	 * this is the lowest matching priority
	 */
	void bind(event_class ec, const action_check_t act);

	/**
	 * lookup an action. If it is bound, execute it.
	 * @return true when the action is executed, false else.
	 */
	bool execute_if_bound(const action_arg_t &e);

private:

	/**
	 * map specific hints
	 */
	std::unordered_map<action_t, action_func_t, action_hash> by_type;

	/**
	 * map specific overriding events
	 */
	std::unordered_map<Event, action_func_t, event_hash> by_event;

	/**
	 * event to action map
	 * event_class as key, to ensure all events can be mapped
	 */
	std::unordered_map<event_class, action_check_t, event_class_hash> by_class;

};

} //namespace input
} //namespace openage

#endif
