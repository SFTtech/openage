// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>
#include <map>
#include <memory>
#include <vector>

#include "error/error.h"
#include "log/message.h"

#include "gamestate/activity/node.h"
#include "gamestate/activity/types.h"
#include "time/time.h"


namespace openage {
namespace event {
class Event;
class EventLoop;
} // namespace event

namespace gamestate {
class GameEntity;
class GameState;

namespace activity {

using event_store_t = std::vector<std::shared_ptr<openage::event::Event>>;


/**
 * Create and register an event on the event loop.
 *
 * When the event is executed, the control flow continues on the branch
 * associated with the event.
 *
 * @param time Time at which the primer function is executed.
 * @param entity Game entity that the activity is assigned to.
 * @param loop Event loop that events are registered on.
 * @param state Game state.
 *
 * @return Event registered on the event loop.
 */
using event_primer_t = std::function<std::shared_ptr<openage::event::Event>(const time::time_t &,
                                                                            const std::shared_ptr<gamestate::GameEntity> &,
                                                                            const std::shared_ptr<event::EventLoop> &,
                                                                            const std::shared_ptr<gamestate::GameState> &)>;

/**
 * Create and register events on the event loop
 *
 * @param time Time at which the primer function is executed.
 * @param entity Game entity that the node is associated with.
 * @param loop Event loop that events are registered on.
 * @param state Game state.
 *
 * @return List of events registered on the event loop.
 */
using event_primer_func_t = std::function<event_store_t(const time::time_t &,
                                                        const std::shared_ptr<gamestate::GameEntity> &,
                                                        const std::shared_ptr<event::EventLoop> &,
                                                        const std::shared_ptr<gamestate::GameState> &)>;

/**
 * Decide which node to visit after the event is handled.
 *
 * @param time Time at which the next function is executed.
 * @param entity Game entity that the node is associated with.
 * @param loop Event loop that events are registered on.
 * @param state Game state.
 *
 * @return ID of the next node to visit.
 */
using event_next_func_t = std::function<node_id_t(const time::time_t &,
                                                  const std::shared_ptr<gamestate::GameEntity> &,
                                                  const std::shared_ptr<event::EventLoop> &,
                                                  const std::shared_ptr<gamestate::GameState> &)>;


/**
 * Default primer function that throws an error.
 */
static const event_primer_func_t no_event = [](const time::time_t &,
                                               const std::shared_ptr<gamestate::GameEntity> &,
                                               const std::shared_ptr<event::EventLoop> &,
                                               const std::shared_ptr<gamestate::GameState> &) {
	throw Error{ERR << "No event primer function registered."};
	return event_store_t{};
};

/**
 * Default next function that throws an error.
 */
static const event_next_func_t no_next = [](const time::time_t &,
                                            const std::shared_ptr<gamestate::GameEntity> &,
                                            const std::shared_ptr<event::EventLoop> &,
                                            const std::shared_ptr<gamestate::GameState> &) {
	throw Error{ERR << "No event next function registered."};
	return 0;
};


/**
 * Waits for an event to be executed before continuing the control flow.
 */
class XorEventGate : public Node {
public:
	/**
      * Create a new exclusive event gateway.
      *
      * @param id Unique identifier for this node.
      * @param label Human-readable label (optional).
      * @param outputs Output nodes (can be set later).
      * @param primer_func Function to create and register the event.
      * @param next_func Function to decide which node to visit after the event is handled.
      */
	[[deprecated]] XorEventGate(node_id_t id,
	                            node_label_t label = "Event",
	                            const std::vector<std::shared_ptr<Node>> &outputs = {},
	                            event_primer_func_t primer_func = no_event,
	                            event_next_func_t next_func = no_next);

	/**
      * Create a new exclusive event gateway.
      *
      * @param id Unique identifier for this node.
      * @param label Human-readable label (optional).
      */
	XorEventGate(node_id_t id,
	             node_label_t label /* = "EventGateWay" */);

	/**
     * Create a new exclusive event gateway.
     *
     * @param id Unique identifier for this node.
     * @param label Human-readable label.
     * @param outputs Output nodes.
     * @param primers Event primers for each output node.
     */
	XorEventGate(node_id_t id,
	             node_label_t label,
	             const std::vector<std::shared_ptr<Node>> &outputs,
	             const std::map<node_id_t, event_primer_t> &primers);

	virtual ~XorEventGate() = default;

	inline node_t get_type() const override {
		return node_t::XOR_EVENT_GATE;
	}

	/**
      * Add an output node.
      *
      * @param output Output node.
      */
	[[deprecated]] void add_output(const std::shared_ptr<Node> &output);

	/**
      * Add an output node.
      *
      * @param output Output node.
      * @param primer Creation function for the event associated with the output node.
      */
	void add_output(const std::shared_ptr<Node> &output,
	                const event_primer_t &primer);

	/**
      * Set the function to create the event.
      *
      * @param primer_func Event creation function.
      */
	[[deprecated]] void set_primer_func(event_primer_func_t primer_func);

	/**
     * Set the function to decide which node to visit after the event is handled.
     *
     * @param next_func Next node function.
     */
	[[deprecated]] void set_next_func(event_next_func_t next_func);

	/**
     * Get the function to create the event.
     *
     * @return Event creation function.
     */
	[[deprecated]] event_primer_func_t get_primer_func() const;

	/**
      * Get the function to decide which node to visit after the event is handled.
      *
      * @return Next node function.
      */
	[[deprecated]] event_next_func_t get_next_func() const;

	/**
     * Get the output->event primer mappings.
     *
     * @return Event primer functions for each output node.
     */
	const std::map<node_id_t, event_primer_t> &get_primers() const;

private:
	/**
     * Creates the event when the node is visited.
     */
	[[deprecated]] event_primer_func_t primer_func;

	/**
     * Decide which node to visit after the event is handled.
     */
	[[deprecated]] event_next_func_t next_func;

	/**
     * Maps output node IDs to event primer functions.
     *
     * Events are created and registered on the event loop when the node is visited.
     */
	std::map<node_id_t, event_primer_t> primers;
};

} // namespace activity
} // namespace gamestate
} // namespace openage
