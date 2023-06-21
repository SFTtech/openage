// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include "gamestate/activity/node.h"


namespace openage::gamestate::activity {

using condition_func_t = std::function<node_id(const curve::time_t &)>;

/**
 * Chooses one of its output nodes based on a condition.
 */
class ConditionNode : public Node {
public:
	/**
     * Creates a new condition node.
     *
     * @param id Unique identifier of the node.
     * @param outputs Output nodes.
     * @param condition_func Function that determines which output node is chosen.
     *                       This must be a valid node ID of one of the output nodes.
     * @param label Label of the node (optional).
     */
	ConditionNode(node_id id,
	              const std::vector<std::shared_ptr<Node>> &outputs,
	              condition_func_t condition_func,
	              node_label label = "Condition");

	virtual ~ConditionNode() = default;

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
