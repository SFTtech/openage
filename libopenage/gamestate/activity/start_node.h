// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include "gamestate/activity/node.h"


namespace openage::gamestate::activity {

/**
 * Start node of an activity. This is where the control flow begins.
 */
class StartNode : public Node {
public:
	/**
	 * Create a new start node.
	 *
	 * @param id Unique identifier for this node.
	 * @param label Human-readable label (optional).
	 * @param output Next node to visit.
	 */
	StartNode(node_id id,
	          const std::shared_ptr<Node> &output,
	          node_label label = "Start");
	virtual ~StartNode() = default;

	/**
	 * Do nothing and return the next node.
	 *
	 * Start nodes only provide the entry for a (sub-)activity. They don't
	 * perform any action.
	 *
	 * @param time Current time.
	 *
	 * @return Next node to visit.
	 */
	virtual const std::shared_ptr<Node> &visit(const curve::time_t &time) const override;
};

} // namespace openage::gamestate::activity
