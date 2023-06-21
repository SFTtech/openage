// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include "gamestate/activity/node.h"


namespace openage {
namespace gamestate::activity {

/* Create and register an event on the event loop */
using event_primer_func_t = std::function<void(const curve::time_t &)>;

/* Decide which node to visit after the event is handled */
using event_next_func_t = std::function<node_id(const curve::time_t &)>;

/**
 * Waits for an event to be executed before continuing the control flow.
 */
class EventNode : public Node {
public:
	EventNode(node_id id,
	          const std::vector<std::shared_ptr<Node>> &outputs,
	          event_primer_func_t primer_func,
	          event_next_func_t next_func,
	          node_label label = "Event");

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
