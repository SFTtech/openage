// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <memory>
#include <string>


namespace openage::gamestate::activity {
class Node;

using activity_id = size_t;
using activity_label = std::string;

/**
 * Encapsules a self-contained control flow as a node graph.
 */
class Activity {
public:
	Activity(activity_id id,
	         activity_label label = "",
	         const std::shared_ptr<Node> &start = {});

	activity_id get_id() const;

	const activity_label get_label() const;

	const std::shared_ptr<Node> &get_start() const;

private:
	const activity_id id;
	const activity_label label;
	std::shared_ptr<Node> start;
};

} // namespace openage::gamestate::activity
