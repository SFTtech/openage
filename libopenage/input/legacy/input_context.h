// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>
#include <stack>
#include <unordered_map>
#include <vector>

#include "input/legacy/action.h"
#include "input/legacy/event.h"

namespace openage {
namespace input::legacy {

class InputManager;


/**
 * An input context contains all keybindings and actions
 * active in e.g. the HUD only.
 * For the console, there's a different input context.
 * That way, each context can have the same keys
 * assigned to different actions, the active context
 * decides, which one to trigger.
 */
class InputContext {
public:
	/**
	 * Create an unbound input context.
	 */
	InputContext();

	/**
	 * Create a bound context, assigned to its manager.
	 */
	InputContext(InputManager *manager);

	virtual ~InputContext() = default;

	/**
	 * a list of all keys of this context
	 * which are bound currently in the active context.
	 *
	 * TODO: move this method to the input manager.
	 *       as InputManager::active_binds(const InputContext &) const;
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

	/**
	 * Called by the InputManager where this context
	 * shall be registered to.
	 */
	void register_to(InputManager *manager);

	/**
	 * Remove the registration to an input manager.
	 */
	void unregister();


	/**
	 * Affects which keyboard events are received:
	 * true to accpet utf8 text events,
	 * false to receive regular char events
	 */
	bool utf8_mode;

private:
	/**
	 * Input manager this context is bound to.
	 */
	InputManager *input_manager;

	/**
	 * Maps an action id to a event execution function.
	 */
	std::unordered_map<action_t, action_func_t> by_type;

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

} // namespace input::legacy
} // namespace openage
