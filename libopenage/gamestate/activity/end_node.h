// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include "gamestate/activity/node.h"


namespace openage::gamestate::activity {

/**
 * End node of an activity. This is where the control flow for an activity
 * ends. When the activity is a subactivity, the control flow returns to the
 * parent activity.
 */
class EndNode : public Node {
public:
	/**
     * Create a new end node.
     *
     * @param id Unique identifier for this node.
     * @param label Human-readable label (optional).
     * @param output Hint for the next node to visit (optional).
     *               This can be ignored by the parent activity.
     */
	EndNode(node_id id,
	        const std::shared_ptr<Node> &output = nullptr,
	        node_label label = "End");
	virtual ~EndNode() = default;

	/**
     * Do nothing and return the next node.
     *
     * End nodes only provide the exit for a (sub-)activity. They don't
     * perform any action.
     *
     * @param time Current time.
     *
     * @return Hint for the next node to visit. Can be \p nullptr.
     *         Can be ignored by the parent activity.
     */
	virtual const std::shared_ptr<Node> &visit(const curve::time_t &time) const override;
};

} // namespace openage::gamestate::activity
