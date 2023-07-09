// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include "gamestate/activity/node.h"


namespace openage::gamestate::activity {

/**
 * End node of an activity. This is where the control flow for an activity
 * ends. When the activity is a subactivity, the control flow returns to the
 * parent activity.
 *
 * inputs: 1
 * outputs: none
 */
class EndNode : public Node {
public:
	/**
     * Create a new end node.
     *
     * @param id Unique identifier for this node.
     * @param label Human-readable label (optional).
     */
	EndNode(node_id id,
	        node_label label = "End");
	virtual ~EndNode() = default;

	inline node_t get_type() const override {
		return node_t::END;
	}

	/**
      * Throws an error since end nodes are not supposed to have outputs
      *
      * @param output Output node.
      *
      * @throws openage::Error
      */
	[[noreturn]] void add_output(const std::shared_ptr<Node> &output) override;
};

} // namespace openage::gamestate::activity
