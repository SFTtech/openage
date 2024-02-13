// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <unordered_map>

#include "input/controller/game/binding.h"
#include "input/event.h"

namespace openage::input::game {

/**
 * Maps input events to gamestate events.
 */
class BindingContext {
public:
	/**
	 * Create a new binding context.
	 */
	BindingContext();

	~BindingContext() = default;

	/**
	 * Bind a specific key combination to a binding.
	 *
	 * This is the first matching priority.
	 *
	 * @param ev Input event triggering the action.
	 * @param bind Binding for the event.
	 */
	void bind(const Event &ev, const binding_action bind);

	/**
	 * Bind an event class to an action.
	 *
	 * This is the second matching priority.
	 *
	 * @param ev Input event triggering the action.
	 * @param bind Binding for the event.
	 */
	void bind(const event_class &cl, const binding_action bind);

	/**
	 * Check whether a specific key event is bound in this context.
	 *
	 * @param ev Input event.
	 *
	 * @return true if event is bound, else false.
	 */
	bool is_bound(const Event &ev) const;

	/**
	 * Get the bindings for a specific event.
	 *
	 * @param ev Input event mapped to the binding.
	 */
	const binding_action &lookup(const Event &ev) const;

private:
	/**
	 * Maps specific input events to bindings.
	 */
	std::unordered_map<Event, binding_action, event_hash> by_event;

	/**
	 * Maps event classes to bindings.
	 */
	std::unordered_map<event_class, binding_action, event_class_hash> by_class;
};

} // namespace openage::input::game
