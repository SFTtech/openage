// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "pathfinder.h"

#include "coord/phys.h"
#include "pathfinding/grid.h"
#include "pathfinding/integrator.h"
#include "pathfinding/portal.h"
#include "pathfinding/sector.h"


namespace openage::path {

Pathfinder::Pathfinder() :
	grids{},
	integrator{std::make_shared<Integrator>()} {
}

const Path Pathfinder::get_path(PathRequest &request) {
	auto portal_path = this->portal_a_star(request);

	// TODO: Implement the rest of the pathfinding process
	return Path{};
}

const std::shared_ptr<Grid> &Pathfinder::get_grid(grid_id_t id) const {
	return this->grids.at(id);
}

void Pathfinder::add_grid(const std::shared_ptr<Grid> &grid) {
	this->grids[grid->get_id()] = grid;
}

const std::vector<std::shared_ptr<Portal>> Pathfinder::portal_a_star(PathRequest &request) const {
	std::vector<std::shared_ptr<Portal>> result;

	auto grid = this->grids.at(request.grid_id);
	auto sector_size = grid->get_sector_size();

	auto start_sector_x = request.start.ne / sector_size;
	auto start_sector_y = request.start.se / sector_size;
	auto start_sector = grid->get_sector(start_sector_x, start_sector_y);

	auto target_sector_x = request.target.ne / sector_size;
	auto target_sector_y = request.target.se / sector_size;
	auto target_sector = grid->get_sector(target_sector_x, target_sector_y);

	// path node storage, always provides cheapest next node.
	heap_t node_candidates;

	// list of known portals and corresponding node.
	nodemap_t visited_portals;

	// Cost to travel from one portal to another
	// TODO: Determine this cost for each portal
	const int distance_cost = 1;

	// start nodes: all portals in the start sector
	for (auto &portal : start_sector->get_portals()) {
		auto portal_node = std::make_shared<PortalNode>(portal, start_sector->get_id(), nullptr);

		auto sector_pos = grid->get_sector(portal->get_exit_sector(start_sector->get_id()))->get_position();
		auto portal_pos = portal->get_exit_center(start_sector->get_id());
		auto portal_abs_pos = portal_pos + coord::tile_delta{sector_pos.ne * sector_size, sector_pos.se * sector_size};
		auto heuristic_cost = Pathfinder::heuristic_cost(portal_abs_pos, request.target);

		portal_node->current_cost = 0;
		portal_node->heuristic_cost = heuristic_cost;
		portal_node->future_cost = portal_node->current_cost + heuristic_cost;

		portal_node->heap_node = node_candidates.push(portal_node);
		visited_portals[portal->get_id()] = portal_node;
	}

	// track the closest we can get to the end position
	// used when no path is found
	auto closest_node = node_candidates.top();

	// while there are candidates to visit
	while (not node_candidates.empty()) {
		auto current_node = node_candidates.pop();

		current_node->was_best = true;

		// check if the current node is the target
		if (current_node->portal->get_exit_sector(current_node->entry_sector) == target_sector->get_id()) {
			auto backtrace = current_node->generate_backtrace();
			for (auto &node : backtrace) {
				result.push_back(node->portal);
			}
			return result;
		}

		// check if the current node is the closest to the target
		if (current_node->heuristic_cost < closest_node->heuristic_cost) {
			closest_node = current_node;
		}

		// get the exits of the current node
		auto exits = current_node->get_exits(visited_portals, current_node->entry_sector);

		// evaluate all neighbors of the current candidate for further progress
		for (auto &exit : exits) {
			if (exit->was_best) {
				continue;
			}

			bool not_visited = !visited_portals.contains(exit->portal->get_id());
			auto tentative_cost = current_node->current_cost + distance_cost;

			if (not_visited or tentative_cost < exit->current_cost) {
				if (not_visited) {
					// calculate the heuristic cost
					exit->heuristic_cost = Pathfinder::heuristic_cost(
						exit->portal->get_exit_center(exit->entry_sector),
						request.target);
				}

				// update the cost knowledge
				exit->current_cost = tentative_cost;
				exit->future_cost = exit->current_cost + exit->heuristic_cost;
				exit->prev_portal = current_node;

				if (not_visited) {
					exit->heap_node = node_candidates.push(exit);
					visited_portals[exit->portal->get_id()] = exit;
				}
				else {
					node_candidates.decrease(exit->heap_node);
				}
			}
		}
	}

	// no path found, return the closest node
	auto backtrace = closest_node->generate_backtrace();
	for (auto &node : backtrace) {
		result.push_back(node->portal);
	}

	return result;
}

int Pathfinder::heuristic_cost(const coord::tile &portal_pos,
                               const coord::tile &target_pos) {
	auto portal_phys_pos = portal_pos.to_phys2();
	auto target_phys_pos = target_pos.to_phys2();
	auto delta = target_phys_pos - portal_phys_pos;

	return delta.length();
}

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

std::vector<node_t> PortalNode::generate_backtrace() {
	std::vector<node_t> waypoints;

	node_t current = this->shared_from_this();
	do {
		node_t other = current;
		waypoints.push_back(current);
		current = current->prev_portal;
	}
	while (current != nullptr);

	return waypoints;
}

std::vector<node_t> PortalNode::get_exits(const nodemap_t &nodes,
                                          sector_id_t entry_sector) {
	std::vector<node_t> exits;

	auto exit_sector = this->portal->get_exit_sector(entry_sector);
	for (auto &exit : this->portal->get_exits(entry_sector)) {
		auto exit_id = exit->get_id();

		if (nodes.contains(exit_id)) {
			exits.push_back(nodes.at(exit_id));
		}
		else {
			exits.push_back(std::make_shared<PortalNode>(exit,
			                                             exit_sector,
			                                             this->shared_from_this()));
		}
	}
	return exits;
}


bool compare_node_cost::operator()(const node_t &lhs, const node_t &rhs) const {
	return *lhs < *rhs;
}

} // namespace openage::path
