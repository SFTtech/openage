// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <unordered_map>

#include "input/action.h"
#include "input/event.h"

namespace openage::input {

class BindingContext;

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
	InputContext(const std::string id);

	virtual ~InputContext() = default;

	/**
     * Get the ID of the context.
     *
     * @return Context ID.
     */
	const std::string &get_id();

	/**
     * Get the associated context for binding input events to game events.
     *
     * @return Binding context of the input context.
     */
	const std::shared_ptr<BindingContext> &get_binding_context();

	/**
	 * bind a specific key event to an action
     *
     * @param ev Input event triggering the action.
     * @param act Function executing the action.
	 */
	void bind(const Event &ev, const InputAction act);

	/**
     * Check whether a specific key event is bound in this context.
     *
     * @param ev Input event.
     *
     * @return true if event is bound, else false.
     */
	bool is_bound(const Event &ev) const;

	/**
     * Get the action(s) bound to a specific event.
     *
     * @param ev Input event triggering the action.
     */
	const InputAction &lookup(const Event &ev) const;

private:
	/**
     * Unique ID of the context.
     */
	std::string id;

	/**
	 * map specific overriding events
	 */
	std::unordered_map<Event, InputAction, event_hash> by_event;

	/**
     * Additional context for gamestate events.
     */
	std::shared_ptr<BindingContext> binding_context;
};

} // namespace openage::input
