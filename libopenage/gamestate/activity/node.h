// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "gamestate/activity/types.h"


namespace openage::gamestate::activity {

using node_id_t = size_t;
using node_label_t = std::string;

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
	Node(node_id_t id,
	     node_label_t label = "",
	     const std::vector<std::shared_ptr<Node>> &outputs = {});
	virtual ~Node() = default;

	/**
	 * Get the type of this node.
	 *
	 * @return Node type.
	 */
	virtual node_t get_type() const = 0;

	/**
	 * Get the unique identifier for this node.
	 *
	 * @return The unique identifier.
	 */
	node_id_t get_id() const;

	/**
	 * Get the human-readable label for this node.
	 *
	 * @return Human-readable label.
	 */
	const node_label_t get_label() const;

	/**
	 * Get a human-readable string representation of this node.
	 *
	 * @return Human-readable representation.
	 */
	std::string str() const;

	/**
	 * Get the output node with the given identifier.
	 *
	 * @param id Unique identifier of the output node.
	 * @return Output node.
	 */
	const std::shared_ptr<Node> &next(node_id_t id) const;

protected:
	/**
	 * Output nodes.
	 */
	std::unordered_map<node_id_t, std::shared_ptr<Node>> outputs;

private:
	/**
	 * Unique identifier for this node.
	 */
	const node_id_t id;

	/**
	 * Human-readable label.
	 */
	const node_label_t label;
};

} // namespace openage::gamestate::activity
