// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <unordered_map>

#include "curve/curve.h"


namespace openage::gamestate::activity {

using node_id = size_t;
using node_label = std::string;

/**
 * Node in the flow graph describing the activity.
 */
class Node {
public:
	/**
	 * Create a new node.
	 *
	 * @param id Unique identifier for this node.
	 * @param label Human-readable label (optional).
	 * @param outputs Output nodes.
	 */
	Node(node_id id,
	     node_label label = "",
	     const std::vector<std::shared_ptr<Node>> &outputs = {});
	virtual ~Node() = default;

	/**
	 * Get the unique identifier for this node.
	 *
	 * @return The unique identifier.
	 */
	node_id get_id() const;

	/**
	 * Get the human-readable label for this node.
	 *
	 * @return Human-readable label.
	 */
	const node_label get_label() const;

	/**
	 * Perform an action like executing a function or evaluating a condition.
	 *
	 * @param time Current time.
	 *
	 * @return Next node to visit.
	 */
	virtual const std::shared_ptr<Node> &visit(const curve::time_t &) const = 0;

protected:
	/**
	 * Output nodes.
	 */
	std::unordered_map<node_id, std::shared_ptr<Node>> outputs;

private:
	/**
	 * Unique identifier for this node.
	 */
	const node_id id;

	/**
	 * Human-readable label.
	 */
	const node_label label;
};

} // namespace openage::gamestate::activity
