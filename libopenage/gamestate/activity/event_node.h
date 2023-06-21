// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include "gamestate/activity/node.h"


namespace openage {
namespace gamestate::activity {

/* Create and register an event on the event loop */
using event_primer_func_t = std::function<void(const curve::time_t &)>;

/* Decide which node to visit after the event is handled */
using event_next_func_t = std::function<node_id(const curve::time_t &)>;


static const event_primer_func_t no_event = [](const curve::time_t &) {
	throw Error{ERR << "No event primer function registered."};
};

static const event_next_func_t no_next = [](const curve::time_t &) {
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
     * Create an event and wait for it to be executed.
     *
     * The control flow must stop until the event is handled. Event handlers
     * need to call \p notify() to continue.
     *
     * @param time Current time.
     *
     * @return Always \p nullptr .
     */
	const std::shared_ptr<Node> &visit(const curve::time_t &time) const override;

	/**
     * Get the next node to visit after the event is handled.
     *
     * @param time Current time.
     *
     * @return Next node to visit.
     */
	const std::shared_ptr<Node> &next(const curve::time_t &time) const;

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

} // namespace gamestate::activity
} // namespace openage
