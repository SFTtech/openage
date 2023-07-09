// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include "gamestate/activity/node.h"


namespace openage::gamestate {
class GameEntity;

namespace activity {

using condition_func_t = std::function<node_id(const curve::time_t &,
                                               const std::shared_ptr<gamestate::GameEntity> &)>;

static const condition_func_t no_condition = [](const curve::time_t &,
                                                const std::shared_ptr<gamestate::GameEntity> &) -> node_id {
	throw Error{MSG(err) << "No condition function set."};
};


/**
 * Chooses one of its output nodes based on a condition.
 */
class ConditionNode : public Node {
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
	ConditionNode(node_id id,
	              node_label label = "Condition",
	              const std::vector<std::shared_ptr<Node>> &outputs = {},
	              condition_func_t condition_func = no_condition);
	virtual ~ConditionNode() = default;

	inline node_t get_type() const override {
		return node_t::XOR_GATEWAY;
	}

	/**
     * Add an output node.
     *
     * @param output Output node.
     */
	void add_output(const std::shared_ptr<Node> &output) override;

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

private:
	/**
     * Determines which output node is chosen.
     */
	condition_func_t condition_func;
};

} // namespace activity
} // namespace openage::gamestate
