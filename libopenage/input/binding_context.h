// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <unordered_map>

#include "input/binding.h"
#include "input/event.h"

namespace openage::input {

/**
 * Maps input events to gamestate events.
 */
class BindingContext {
public:
	/**
	 * Create a binding context.
	 */
	BindingContext();

	virtual ~BindingContext() = default;

	/**
     * Get the ID of the context.
     *
     * @return Context ID.
     */
	const std::string &get_id();

	/**
	 * bind a specific key event to an action
     *
     * @param ev Input event triggering the action.
     * @param bind Binding for the event.
	 */
	void bind(const KeyEvent &ev, const Binding bind);
	void bind(const MouseEvent &ev, const Binding bind);
	void bind(const WheelEvent &ev, const Binding bind);

	/**
     * Check whether a specific key event is bound in this context.
     *
     * @param ev Input event.
     *
     * @return true if event is bound, else false.
     */
	bool is_bound(const KeyEvent &ev) const;
	bool is_bound(const MouseEvent &ev) const;
	bool is_bound(const WheelEvent &ev) const;

	/**
     * Get the bindings for a specific event.
     *
     * @param ev Input event mapped to the binding.
     */
	const Binding &lookup(const KeyEvent &ev) const;
	const Binding &lookup(const MouseEvent &ev) const;
	const Binding &lookup(const WheelEvent &ev) const;

	// ASDF: Up and downcasting is a really bad way to do this!
	const Binding &lookup(const Event &ev) const;

private:
	/**
	 * map specific overriding events
	 */
	std::unordered_map<KeyEvent, Binding, event_hash> by_keyevent;
	std::unordered_map<MouseEvent, Binding, event_hash> by_mouseevent;
	std::unordered_map<WheelEvent, Binding, event_hash> by_wheelevent;
};

} // namespace openage::input
