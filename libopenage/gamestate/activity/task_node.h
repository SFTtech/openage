// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>
#include <memory>

#include "error/error.h"
#include "log/message.h"

#include "gamestate/activity/node.h"
#include "gamestate/activity/types.h"
#include "time/time.h"


namespace openage::gamestate {
class GameEntity;

namespace activity {

using task_func_t = std::function<void(const time::time_t &,
                                       const std::shared_ptr<gamestate::GameEntity> &)>;

static const task_func_t no_task = [](const time::time_t &,
                                      const std::shared_ptr<gamestate::GameEntity> &) {
	throw Error{ERR << "No task defined for this node."};
};

/**
 * Executes a function when visited.
 */
class TaskCustom : public Node {
public:
	/**
	 * Create a new task node.
	 *
	 * @param id Unique identifier for this node.
	 * @param label Human-readable label (optional).
	 * @param task_func Action to perform when visiting this node (can be set later).
	 * @param output Next node to visit (optional).
	 */
	TaskCustom(node_id_t id,
	           node_label_t label = "TaskCustom",
	           const std::shared_ptr<Node> &output = nullptr,
	           task_func_t task_func = no_task);
	virtual ~TaskCustom() = default;

	inline node_t get_type() const override {
		return node_t::TASK_CUSTOM;
	}

	/**
	 * Set the current output node.
	 *
	 * @param output Output node.
	 */
	void add_output(const std::shared_ptr<Node> &output);

	/**
	 * Set the task function.
	 *
	 * @param task_func Action to perform when visiting this node.
	 */
	void set_task_func(task_func_t task_func);

	/**
	 * Get the task function.
	 *
	 * @return Action to perform when visiting this node.
	 */
	task_func_t get_task_func() const;

	/**
	 * Get the next node to visit.
	 *
	 * @param time Current time.
	 * @return Next node to visit.
	 */
	node_id_t get_next() const;

private:
	/**
	 * Action to perform when visiting this node.
	 */
	task_func_t task_func;
};

} // namespace activity
} // namespace openage::gamestate
