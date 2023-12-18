// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <vector>

#include "error/error.h"
#include "log/message.h"

#include "gamestate/activity/node.h"
#include "gamestate/activity/types.h"
#include "time/time.h"


namespace openage::gamestate {
class GameEntity;

namespace activity {

/**
 * Function that determines if an output node is chosen.
 *
 * @param time Current game time.
 * @param entity Entity that is executing the activity.
 *
 * @return true if the output node is chosen, false otherwise.
 */
using condition_t = std::function<bool(const time::time_t &,
                                       const std::shared_ptr<gamestate::GameEntity> &)>;

/**
 * Function that determines which output node is chosen.
 *
 * @param time Current game time.
 * @param entity Entity that is executing the activity.
 * @return ID of the output node that is chosen.
 */
using condition_func_t = std::function<node_id_t(const time::time_t &,
                                                 const std::shared_ptr<gamestate::GameEntity> &)>;

/**
 * Default condition function that throws an error.
 */
static const condition_func_t no_condition = [](const time::time_t &,
                                                const std::shared_ptr<gamestate::GameEntity> &) -> node_id_t {
	throw Error{MSG(err) << "No condition function set."};
};


/**
 * Chooses one of its output nodes based on conditions.
 */
class XorGate : public Node {
public:
	/**
     * Creates a new condition node.
     *
     * @param id Unique identifier of the node.
     * @param label Label of the node (optional).
     * @param outputs Output nodes (can be set later).
     * @param condition_func Function that determines which output node is chosen (can be set later).
     *                       This must be a valid node ID of one of the output nodes.
     */
	XorGate(node_id_t id,
	        node_label_t label = "ExclusiveGateway",
	        const std::vector<std::shared_ptr<Node>> &outputs = {},
	        condition_func_t condition_func = no_condition);

	/**
     * Creates a new condition node.
     *
     * @param id Unique identifier of the node.
     * @param label Label of the node.
     * @param outputs Output nodes.
     * @param conditions Conditions for each output node.
     * @param default_id Default output node ID.
     */
	XorGate(node_id_t id,
	        node_label_t label,
	        const std::vector<std::shared_ptr<Node>> &outputs,
	        const std::vector<condition_t> &conditions,
	        const node_id_t default_id);

	virtual ~XorGate() = default;

	inline node_t get_type() const override {
		return node_t::XOR_GATE;
	}

	/**
     * Add an output node.
     *
     * @param output Output node.
     */
	void add_output(const std::shared_ptr<Node> &output) override;

	/**
     * Add an output node.
     *
     * @param output Output node.
     * @param condition_func Function that determines whether this output node is chosen.
     *                       This must be a valid node ID of one of the output nodes.
     */
	void add_output(const std::shared_ptr<Node> &output,
	                const condition_t condition_func);

	/**
     * Set the function that determines which output node is chosen.
     *
     * @param condition_func Function that determines which output node is chosen.
     *                       This must be a valid node ID of one of the output nodes.
     */
	void set_condition_func(condition_func_t condition_func);

	/**
     * Get the function that determines which output node is chosen.
     *
     * @return Function that determines which output node is chosen.
     */
	condition_func_t get_condition_func() const;

	/**
     * Get the output->condition mappings.
     *
     * @return Conditions for each output node.
     */
	const std::map<node_id_t, condition_t> &get_conditions() const;

	/**
     * Get the ID of the default output node.
     *
     * @return Default output node ID.
     */
	node_id_t get_default_id() const;

	/**
     * Set the ID of the default output node.
     *
     * The ID must be a valid node ID of one of the output nodes.
     *
     * @param id Default output node ID.
     */
	void set_default_id(node_id_t id);

private:
	/**
     * Determines which output node is chosen.
     */
	condition_func_t condition_func;

	/**
     * Maps output node IDs to condition functions.
     *
     * Conditions are checked in order they appear in the map.
     */
	std::map<node_id_t, condition_t> conditions;

	/**
     * Default output node ID. Chosen if no condition is true.
     */
	std::optional<node_id_t> default_id;
};

} // namespace activity
} // namespace openage::gamestate
