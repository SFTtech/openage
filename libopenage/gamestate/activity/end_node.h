// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include "gamestate/activity/node.h"
#include "gamestate/activity/types.h"


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
	EndNode(node_id_t id,
	        node_label_t label = "End");
	virtual ~EndNode() = default;

	inline node_t get_type() const override {
		return node_t::END;
	}
};

} // namespace openage::gamestate::activity
