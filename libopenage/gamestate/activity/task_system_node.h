// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include "gamestate/activity/node.h"
#include "gamestate/activity/types.h"
#include "gamestate/system/types.h"


namespace openage::gamestate::activity {

/**
 * Similar to the custom task node, but runs a built-in system function
 * when visited.
 */
class TaskSystemNode : public Node {
public:
	/**
	 * Create a new custom task system node.
	 *
	 * @param id Unique identifier for this node.
	 * @param label Human-readable label (optional).
	 * @param output Next node to visit (optional).
	 * @param system_id System to run when visiting this node (can be set later).
	 */
	TaskSystemNode(node_id_t id,
	               node_label_t label = "TaskSystem",
	               const std::shared_ptr<Node> &output = nullptr,
	               system::system_id_t system_id = system::system_id_t::NONE);
	virtual ~TaskSystemNode() = default;

	inline node_t get_type() const override {
		return node_t::TASK_SYSTEM;
	}

	/**
	 * Set the current output node.
	 *
	 * @param output Output node.
	 */
	void add_output(const std::shared_ptr<Node> &output);

	/**
	 * Set the system id.
	 *
	 * @param system_id System to run when visiting this node.
	 */
	void set_system_id(system::system_id_t system_id);

	/**
	 * Get the system id.
	 *
	 * @return System to run when visiting this node.
	 */
	system::system_id_t get_system_id() const;

	/**
	 * Get the next node to visit.
	 *
	 * @param time Current time.
	 * @return Next node to visit.
	 */
	node_id_t get_next() const;

private:
	/**
	 * System to run when visiting this node.
	 */
	system::system_id_t system_id;
};


} // namespace openage::gamestate::activity
