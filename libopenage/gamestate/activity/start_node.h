// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include "gamestate/activity/node.h"
#include "gamestate/activity/types.h"


namespace openage::gamestate::activity {

/**
 * Start node of an activity. This is where the control flow begins.
 *
 * inputs: none
 * outputs: 1
 */
class StartNode : public Node {
public:
	/**
	 * Create a new start node.
	 *
	 * @param id Unique identifier for this node.
	 * @param label Human-readable label (optional).
	 * @param output Next node to visit (can be set later).
	 */
	StartNode(node_id_t id,
	          node_label_t label = "Start",
	          const std::shared_ptr<Node> &output = nullptr);
	virtual ~StartNode() = default;

	inline node_t get_type() const override {
		return node_t::START;
	}

	/**
	 * Set the current output node.
	 *
	 * Replace any output node that has been set before.
	 *
	 * @param output Output node.
	 */
	void add_output(const std::shared_ptr<Node> &output);

	/**
	 * Get the next node to visit.
	 *
	 * @param time Current time.
	 * @return Next node to visit.
	 */
	node_id_t get_next() const;
};

} // namespace openage::gamestate::activity
