// Copyright 2021-2025 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <vector>

#include "curve/discrete.h"
#include "gamestate/component/internal_component.h"
#include "gamestate/component/types.h"
#include "time/time.h"


namespace openage {

namespace event {
class Event;
class EventLoop;
} // namespace event

namespace gamestate {

namespace activity {
class Activity;
class Node;
} // namespace activity

namespace component {

/**
 * Store the activity flow graph of a game entity.
 */
class Activity final : public InternalComponent {
public:
	/**
	 * Creates a new activity component.
	 *
	 * @param loop Event loop that all events from the component are registered on.
	 * @param activity Activity flow graph.
	 */
	Activity(const std::shared_ptr<openage::event::EventLoop> &loop,
	         const std::shared_ptr<activity::Activity> &activity);

	component_t get_type() const override;

	/**
	 * Get the activity graph of the component.
	 *
	 * @return Activity graph.
	 */
	const std::shared_ptr<activity::Activity> &get_start_activity() const;

	/**
	 * Get the node in the activity flow graph at a given time.
	 *
	 * @param time Time at which the node is requested.
	 *
	 * @return Node in the flow graph.
	 */
	const std::shared_ptr<activity::Node> get_node(const time::time_t &time) const;

	/**
	 * Sets the node in the activity flow graph at a given time.
	 *
	 * @param time Time at which the node is set.
	 * @param node Node in the flow graph.
	 */
	void set_node(const time::time_t &time,
	              const std::shared_ptr<activity::Node> &node);

	/**
	 * Initialize the activity flow graph for the component at a given time.
	 *
	 * This sets the current node at \p time to the start node of the activity graph.
	 *
	 * @param time Time at which the component is initialized.
	 */
	void init(const time::time_t &time);

	/**
	 * Store a scheduled event that the activity system waits for to
	 * progress in the node graph.
	 *
	 * @param event Event to add.
	 */
	void add_event(const std::shared_ptr<openage::event::Event> &event);

	/**
	 * Cancel all stored scheduled events.
	 *
	 * @param time Time at which the events are cancelled.
	 */
	void cancel_events(const time::time_t &time);

private:
	/**
	 * Activity that encapsulates the entity's control flow graph.
	 *
	 * When a game entity is spawned, the activity system should advance in
	 * this activity flow graph to initialize the entity's action state.
	 *
	 * TODO: Define as curve, so it's changeable?
	 */
	std::shared_ptr<activity::Activity> start_activity;

	/**
	 * Current active node in the activity flow graph over time.
	 */
	curve::Discrete<std::shared_ptr<activity::Node>> node;

	/**
	 * Scheduled events that the actvity system waits for.
	 */
	std::vector<std::shared_ptr<openage::event::Event>> scheduled_events;
};

} // namespace component
} // namespace gamestate
} // namespace openage
