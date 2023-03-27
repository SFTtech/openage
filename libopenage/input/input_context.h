// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <unordered_map>

#include "input/action.h"
#include "input/event.h"

namespace openage::input {

namespace camera {
class BindingContext;
}

namespace engine {
class BindingContext;
}

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
	 *
	 * @param id Unique identifier.
	 */
	InputContext(const std::string id);

	virtual ~InputContext() = default;

	/**
     * Get the unique ID of the context.
     *
     * @return Context ID.
     */
	const std::string &get_id();

	/**
     * Get the associated context for binding input events to game events.
     *
     * @return Binding context of the input context.
     */
	const std::shared_ptr<engine::BindingContext> &get_engine_bindings();

	/**
     * Get the associated context for binding input events to camera actions.
     *
     * @return Binding context of the input context.
     */
	const std::shared_ptr<camera::BindingContext> &get_camera_bindings();

	/**
	 * Bind a specific key combination to an action.
	 *
	 * This is the first matching priority.
     *
     * @param ev Input event triggering the action.
     * @param act Function executing the action.
	 */
	void bind(const Event &ev, const input_action act);

	/**
	 * Bind an event class to an action.
	 *
	 * This is the second matching priority.
     *
     * @param ev Input event triggering the action.
     * @param act Function executing the action.
	 */
	void bind(const event_class &cl, const input_action act);

	/**
     * Check whether a specific key event is bound in this context.
     *
     * @param ev Input event.
     *
     * @return true if event is bound, else false.
     */
	bool is_bound(const Event &ev) const;

	/**
     * Get the action(s) bound to the given event.
     *
     * @param ev Input event triggering the action.
     */
	const input_action &lookup(const Event &ev) const;

	/**
	 * Get all event->action bindings in this context.
	 *
	 * @return Events bound in this context.
	 */
	std::vector<Event> get_event_binds() const;

	/**
	 * Get all class->action bindings in this context.
	 *
	 * @return Event classes bound in this context.
	 */
	std::vector<event_class> get_class_binds() const;


private:
	/**
     * Unique ID of the context.
     */
	std::string id;

	/**
	 * Maps specific input events to actions.
	 */
	std::unordered_map<Event, input_action, event_hash> by_event;

	/**
	 * Maps event classes to actions.
	 */
	std::unordered_map<event_class, input_action, event_class_hash> by_class;

	/**
     * Additional context for engine events.
     */
	std::shared_ptr<engine::BindingContext> binding_context;

	/**
     * Additional context for camera actions.
     */
	std::shared_ptr<camera::BindingContext> camera_bindings;
};

} // namespace openage::input
