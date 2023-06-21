// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include "gamestate/activity/node.h"


namespace openage::gamestate::activity {

using condition_func_t = std::function<node_id(const curve::time_t &)>;

static const condition_func_t no_condition = [](const curve::time_t &) -> node_id {
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
     * Checks the condition and returns the next node that matches the condition.
     *
     * Conditions are evaluated immediately when visiting the node.
     *
     * @param time Current time.
     *
     * @return Next node to visit.
     */
	const std::shared_ptr<Node> &visit(const curve::time_t &time) const override;

private:
	/**
     * Determines which output node is chosen.
     */
	condition_func_t condition_func;
};

} // namespace openage::gamestate::activity
