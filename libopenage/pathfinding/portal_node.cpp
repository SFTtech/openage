// Copyright 2025-2025 the openage authors. See copying.md for legal info.

#include "coord/phys.h"

#include "portal_node.h"


namespace openage::path {

PortalNode::PortalNode(const std::shared_ptr<Portal> &portal) :
	portal{portal},
	entry_sector{std::nullopt},
	future_cost{std::numeric_limits<int>::max()},
	current_cost{std::numeric_limits<int>::max()},
	heuristic_cost{std::numeric_limits<int>::max()},
	was_best{false},
	prev_portal{nullptr},
	heap_node{nullptr} {}

PortalNode::PortalNode(const std::shared_ptr<Portal> &portal,
                       sector_id_t entry_sector,
                       const node_t &prev_portal) :
	portal{portal},
	entry_sector{entry_sector},
	future_cost{std::numeric_limits<int>::max()},
	current_cost{std::numeric_limits<int>::max()},
	heuristic_cost{std::numeric_limits<int>::max()},
	was_best{false},
	prev_portal{prev_portal},
	heap_node{nullptr} {}

PortalNode::PortalNode(const std::shared_ptr<Portal> &portal,
                       sector_id_t entry_sector,
                       const node_t &prev_portal,
                       int past_cost,
                       int heuristic_cost) :
	portal{portal},
	entry_sector{entry_sector},
	future_cost{past_cost + heuristic_cost},
	current_cost{past_cost},
	heuristic_cost{heuristic_cost},
	was_best{false},
	prev_portal{prev_portal},
	heap_node{nullptr} {
}

bool PortalNode::operator<(const PortalNode &other) const {
	return this->future_cost < other.future_cost;
}

bool PortalNode::operator==(const PortalNode &other) const {
	return this->portal->get_id() == other.portal->get_id();
}

std::vector<PortalNode::node_t> PortalNode::generate_backtrace() {
	std::vector<node_t> waypoints;

	node_t current = this->shared_from_this();
	do {
		waypoints.push_back(current);
		current = current->prev_portal;
	}
	while (current != nullptr);

	return waypoints;
}

const PortalNode::exits_t &PortalNode::get_exits(sector_id_t entry_sector) {
	ENSURE(entry_sector == this->node_sector_0 || entry_sector == this->node_sector_1, "Invalid entry sector");

	if (this->node_sector_0 == entry_sector) {
		return exits_1;
	}
	else {
		return exits_0;
	}
}

bool PortalNode::compare_node_cost::operator()(const PortalNode::node_t &lhs, const PortalNode::node_t &rhs) const {
	return *lhs < *rhs;
}

int get_distance_cost(const coord::tile_delta &portal1_pos, const coord::tile_delta &portal2_pos) {
	auto delta = portal2_pos.to_phys2() - portal1_pos.to_phys2();
	return delta.length();
}

}
