// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include "gamestate/activity/node.h"


namespace openage::gamestate {
class GameEntity;

namespace activity {

/* Create and register an event on the event loop */
using event_primer_func_t = std::function<void(const curve::time_t &,
                                               const std::shared_ptr<gamestate::GameEntity> &)>;

/* Decide which node to visit after the event is handled */
using event_next_func_t = std::function<node_id(const curve::time_t &,
                                                const std::shared_ptr<gamestate::GameEntity> &)>;


static const event_primer_func_t no_event = [](const curve::time_t &,
                                               const std::shared_ptr<gamestate::GameEntity> &) {
	throw Error{ERR << "No event primer function registered."};
};

static const event_next_func_t no_next = [](const curve::time_t &,
                                            const std::shared_ptr<gamestate::GameEntity> &) {
	throw Error{ERR << "No event next function registered."};
	return 0;
};


/**
 * Waits for an event to be executed before continuing the control flow.
 */
class EventNode : public Node {
public:
	/**
      * Create a new event node.
      *
      * @param id Unique identifier for this node.
      * @param label Human-readable label (optional).
      * @param outputs Output nodes (can be set later).
      * @param primer_func Function to create and register the event.
      * @param next_func Function to decide which node to visit after the event is handled.
      */
	EventNode(node_id id,
	          node_label label = "Event",
	          const std::vector<std::shared_ptr<Node>> &outputs = {},
	          event_primer_func_t primer_func = no_event,
	          event_next_func_t next_func = no_next);
	virtual ~EventNode() = default;

	inline node_t get_type() const override {
		return node_t::EVENT_GATEWAY;
	}

	/**
      * Add an output node.
      *
      * @param output Output node.
      */
	void add_output(const std::shared_ptr<Node> &output) override;

	/**
      * Set the function to create the event.
      *
      * @param primer_func Event creation function.
      */
	void set_primer_func(event_primer_func_t primer_func);

	/**
     * Set the function to decide which node to visit after the event is handled.
     *
     * @param next_func Next node function.
     */
	void set_next_func(event_next_func_t next_func);

	/**
     * Get the function to create the event.
     *
     * @return Event creation function.
     */
	event_primer_func_t get_primer_func() const;

	/**
      * Get the function to decide which node to visit after the event is handled.
      *
      * @return Next node function.
      */
	event_next_func_t get_next_func() const;

private:
	/**
     * Creates the event when the node is visited.
     */
	event_primer_func_t primer_func;

	/**
     * Decide which node to visit after the event is handled.
     */
	event_next_func_t next_func;
};

} // namespace activity
} // namespace openage::gamestate
