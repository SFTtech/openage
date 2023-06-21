// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>
#include <memory>

#include "gamestate/activity/node.h"


namespace openage::gamestate::activity {

using task_func_t = std::function<void(const curve::time_t &)>;


/**
 * Executes a function when visited.
 */
class TaskNode : public Node {
public:
	/**
     * Create a new task node.
     *
     * @param id Unique identifier for this node.
     * @param label Human-readable label (optional).
     * @param output Next node to visit.
     * @param task Action to perform when visiting this node.
     */
	TaskNode(node_id id,
	         const std::shared_ptr<Node> &output,
	         task_func_t task,
	         node_label label = "Task");
	virtual ~TaskNode() = default;

	/**
     * Perform the action of this node and return the next node.
     *
     * @param time Current time.
     *
     * @return Next node to visit.
     */
	virtual const std::shared_ptr<Node> &visit(const curve::time_t &time) const override;

private:
	/**
     * Action to perform when visiting this node.
     */
	task_func_t task;
};

} // namespace openage::gamestate::activity
