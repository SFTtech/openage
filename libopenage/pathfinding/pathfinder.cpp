// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "pathfinder.h"

#include "coord/chunk.h"
#include "coord/phys.h"
#include "error/error.h"
#include "pathfinding/cost_field.h"
#include "pathfinding/flow_field.h"
#include "pathfinding/grid.h"
#include "pathfinding/integration_field.h"
#include "pathfinding/integrator.h"
#include "pathfinding/portal.h"
#include "pathfinding/sector.h"


namespace openage::path {

Pathfinder::Pathfinder() :
	grids{},
	integrator{std::make_shared<Integrator>()} {
}

const Path Pathfinder::get_path(const PathRequest &request) {
	auto grid = this->grids.at(request.grid_id);
	auto sector_size = grid->get_sector_size();

	// Check if the target is within the grid
	auto grid_size = grid->get_size();
	auto grid_width = grid_size[0] * sector_size;
	auto grid_height = grid_size[1] * sector_size;
	if (request.target.ne < 0
	    or request.target.se < 0
	    or request.target.ne >= static_cast<coord::tile_t>(grid_width)
	    or request.target.se >= static_cast<coord::tile_t>(grid_height)) {
		log::log(DBG << "Path not found (start = "
		             << request.start << "; target = "
		             << request.target << "): "
		             << "Target is out of bounds.");
		return Path{request.grid_id, PathResult::OUT_OF_BOUNDS, {}};
	}

	auto start_sector_x = request.start.ne / sector_size;
	auto start_sector_y = request.start.se / sector_size;
	auto start_sector = grid->get_sector(start_sector_x, start_sector_y);

	auto target_sector_x = request.target.ne / sector_size;
	auto target_sector_y = request.target.se / sector_size;
	auto target_sector = grid->get_sector(target_sector_x, target_sector_y);

	auto target = request.target - target_sector->get_position().to_tile(sector_size);
	if (target_sector->get_cost_field()->get_cost(target) == COST_IMPASSABLE) {
		// TODO: This may be okay if the target is a building or unit
		log::log(DBG << "Path not found (start = "
		             << request.start << "; target = "
		             << request.target << "): "
		             << "Target is impassable.");
		return Path{request.grid_id, PathResult::NOT_FOUND, {}};
	}

	// Integrate the target field
	coord::tile_delta target_delta = request.target - target_sector->get_position().to_tile(sector_size);
	auto target_integration_field = this->integrator->integrate(target_sector->get_cost_field(),
	                                                            target_delta);

	if (target_sector == start_sector) {
		auto start = request.start - start_sector->get_position().to_tile(sector_size);

		if (target_integration_field->get_cell(start.ne, start.se).cost != INTEGRATED_COST_UNREACHABLE) {
			// Exit early if the start and target are in the same sector
			// and are reachable from within the same sector
			auto flow_field = this->integrator->build(target_integration_field);
			auto flow_field_waypoints = this->get_waypoints({std::make_pair(target_sector->get_id(), flow_field)}, request);

			std::vector<coord::tile> waypoints{};
			if (flow_field_waypoints.at(0) != request.start) {
				waypoints.push_back(request.start);
			}
			waypoints.insert(waypoints.end(), flow_field_waypoints.begin(), flow_field_waypoints.end());

			log::log(DBG << "Path found (start = "
			             << request.start << "; target = "
			             << request.target << "): "
			             << "Path is within the same sector.");
			return Path{request.grid_id, PathResult::FOUND, waypoints};
		}
	}

	// Check which portals are reachable from the target field
	std::unordered_set<portal_id_t> target_portal_ids;
	for (auto &portal : target_sector->get_portals()) {
		auto center_cell = portal->get_entry_center(target_sector->get_id());

		if (target_integration_field->get_cell(center_cell).cost != INTEGRATED_COST_UNREACHABLE) {
			target_portal_ids.insert(portal->get_id());
		}
	}

	// Check which portals are reachable from the start field
	coord::tile_delta start = request.start - start_sector->get_position().to_tile(sector_size);
	auto start_integration_field = this->integrator->integrate(start_sector->get_cost_field(),
	                                                           start,
	                                                           false);

	std::unordered_set<portal_id_t> start_portal_ids;
	for (auto &portal : start_sector->get_portals()) {
		auto center_cell = portal->get_entry_center(start_sector->get_id());

		if (start_integration_field->get_cell(center_cell).cost != INTEGRATED_COST_UNREACHABLE) {
			start_portal_ids.insert(portal->get_id());
		}
	}

	if (target_portal_ids.empty() or start_portal_ids.empty()) {
		// Exit early if no portals are reachable from the start or target
		log::log(DBG << "Path not found (start = "
		             << request.start << "; target = "
		             << request.target << "): "
		             << "No portals are reachable from the start or target.");
		return Path{request.grid_id, PathResult::NOT_FOUND, {}};
	}

	// High-level pathfinding
	// Find the portals to use to get from the start to the target
	auto portal_result = this->portal_a_star(request, target_portal_ids, start_portal_ids);
	auto portal_status = portal_result.first;
	auto portal_path = portal_result.second;

	// Low-level pathfinding
	// Find the path within the sectors

	// Build flow field for the target sector
	auto prev_integration_field = target_integration_field;
	auto prev_flow_field = this->integrator->build(prev_integration_field);
	auto prev_sector_id = target_sector->get_id();

	Integrator::get_return_t sector_fields{prev_integration_field, prev_flow_field};

	std::vector<std::pair<sector_id_t, std::shared_ptr<FlowField>>> flow_fields;
	flow_fields.reserve(portal_path.size() + 1);
	flow_fields.push_back(std::make_pair(target_sector->get_id(), sector_fields.second));

	int los_depth = 1;

	for (auto &portal : portal_path) {
		auto prev_sector = grid->get_sector(prev_sector_id);
		auto next_sector_id = portal->get_exit_sector(prev_sector_id);
		auto next_sector = grid->get_sector(next_sector_id);

		target_delta = request.target - next_sector->get_position().to_tile(sector_size);
		bool with_los = los_depth > 0;

		sector_fields = this->integrator->get(next_sector->get_cost_field(),
		                                      prev_integration_field,
		                                      prev_sector_id,
		                                      portal,
		                                      target_delta,
		                                      with_los);
		flow_fields.push_back(std::make_pair(next_sector_id, sector_fields.second));

		prev_integration_field = sector_fields.first;
		prev_sector_id = next_sector_id;
		los_depth -= 1;
	}

	// reverse the flow fields so they are ordered from start to target
	std::reverse(flow_fields.begin(), flow_fields.end());

	// traverse the flow fields to get the waypoints
	auto flow_field_waypoints = this->get_waypoints(flow_fields, request);
	std::vector<coord::tile> waypoints{};
	if (flow_field_waypoints.at(0) != request.start) {
		waypoints.push_back(request.start);
	}
	waypoints.insert(waypoints.end(), flow_field_waypoints.begin(), flow_field_waypoints.end());

	if (portal_status == PathResult::NOT_FOUND) {
		log::log(DBG << "Path not found (start = "
		             << request.start << "; target = "
		             << request.target << ")");
	}
	else {
		log::log(DBG << "Path found (start = "
		             << request.start << "; target = "
		             << request.target << ")");
	}

	return Path{request.grid_id, portal_status, waypoints};
}

const std::shared_ptr<Grid> &Pathfinder::get_grid(grid_id_t id) const {
	return this->grids.at(id);
}

void Pathfinder::add_grid(const std::shared_ptr<Grid> &grid) {
	this->grids[grid->get_id()] = grid;
}

const Pathfinder::portal_star_t Pathfinder::portal_a_star(const PathRequest &request,
                                                          const std::unordered_set<portal_id_t> &target_portal_ids,
                                                          const std::unordered_set<portal_id_t> &start_portal_ids) const {
	std::vector<std::shared_ptr<Portal>> result;

	auto grid = this->grids.at(request.grid_id);
	auto &portal_map = grid->get_portal_map();
	auto sector_size = grid->get_sector_size();

	auto start_sector_x = request.start.ne / sector_size;
	auto start_sector_y = request.start.se / sector_size;
	auto start_sector = grid->get_sector(start_sector_x, start_sector_y);

	// path node storage, always provides cheapest next node.
	heap_t node_candidates;

	std::unordered_set<portal_id_t> visited_portals;

	// TODO: Compute cost to travel from one portal to another when creating portals
	// const int distance_cost = 1;

	// create start nodes
	for (auto &portal : start_sector->get_portals()) {
		if (not start_portal_ids.contains(portal->get_id())) {
			// only consider portals that are reachable from the start cell
			continue;
		}

		auto &portal_node = portal_map.at(portal->get_id());
		portal_node->entry_sector = start_sector->get_id();

		auto sector_pos = grid->get_sector(portal->get_exit_sector(start_sector->get_id()))->get_position().to_tile(sector_size);
		auto portal_pos = portal->get_exit_center(start_sector->get_id());
		auto portal_abs_pos = sector_pos + portal_pos;
		auto heuristic_cost = Pathfinder::heuristic_cost(portal_abs_pos, request.target);

		portal_node->current_cost = 0;
		portal_node->heuristic_cost = heuristic_cost;
		portal_node->future_cost = portal_node->current_cost + heuristic_cost;

		portal_node->heap_node = node_candidates.push(portal_node);
		portal_node->prev_portal = nullptr;
		portal_node->was_best = false;
		visited_portals.insert(portal->get_id());
	}

	// track the closest we can get to the end position
	// used when no path is found
	auto closest_node = node_candidates.top();

	// while there are candidates to visit
	while (not node_candidates.empty()) {
		auto current_node = node_candidates.pop();

		current_node->was_best = true;

		// check if the current node is a portal in the target sector that can
		// be reached from the target cell
		auto exit_portal_id = current_node->portal->get_id();
		if (target_portal_ids.contains(exit_portal_id)) {
			auto backtrace = current_node->generate_backtrace();
			for (auto &node : backtrace) {
				result.push_back(node->portal);
			}
			log::log(DBG << "Portal path found with " << result.size() << " portal traversals.");
			return std::make_pair(PathResult::FOUND, result);
		}

		// check if the current node is the closest to the target
		if (current_node->heuristic_cost < closest_node->heuristic_cost) {
			closest_node = current_node;
		}

		// get the exits of the current node
		const auto &exits = current_node->get_exits(current_node->entry_sector);

		// evaluate all neighbors of the current candidate for further progress
		for (auto &[exit, distance_cost] : exits) {
			exit->entry_sector = current_node->portal->get_exit_sector(current_node->entry_sector);
			bool not_visited = !visited_portals.contains(exit->portal->get_id());

			if (not_visited) {
				exit->was_best = false;
			}
			else if (exit->was_best) {
				continue;
			}


			auto tentative_cost = current_node->current_cost + distance_cost;

			if (not_visited or tentative_cost < exit->current_cost) {
				if (not_visited) {
					// Get heuristic cost (from exit node to target cell)
					auto exit_sector = grid->get_sector(exit->portal->get_exit_sector(exit->entry_sector));
					auto exit_sector_pos = exit_sector->get_position().to_tile(sector_size);
					auto exit_portal_pos = exit->portal->get_exit_center(exit->entry_sector);
					exit->heuristic_cost = Pathfinder::heuristic_cost(
						exit_sector_pos + exit_portal_pos,
						request.target);
				}

				// update the cost knowledge
				exit->current_cost = tentative_cost;
				exit->future_cost = exit->current_cost + exit->heuristic_cost;
				exit->prev_portal = current_node;

				if (not_visited) {
					exit->heap_node = node_candidates.push(exit);
					visited_portals.insert(exit->portal->get_id());
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

	log::log(DBG << "Portal path not found.");
	log::log(DBG << "Closest portal: " << closest_node->portal->get_id());
	return std::make_pair(PathResult::NOT_FOUND, result);
}

const std::vector<coord::tile> Pathfinder::get_waypoints(const std::vector<std::pair<sector_id_t, std::shared_ptr<FlowField>>> &flow_fields,
                                                         const PathRequest &request) const {
	ENSURE(flow_fields.size() > 0, "At least 1 flow field is required for finding waypoints.");

	std::vector<coord::tile> waypoints;

	auto grid = this->get_grid(request.grid_id);
	auto sector_size = grid->get_sector_size();
	coord::tile_t start_x = request.start.ne % sector_size;
	coord::tile_t start_y = request.start.se % sector_size;

	bool los_reached = false;

	coord::tile_t current_x = start_x;
	coord::tile_t current_y = start_y;
	flow_dir_t current_direction = flow_fields.at(0).second->get_dir(current_x, current_y);
	for (size_t i = 0; i < flow_fields.size(); ++i) {
		auto &sector = grid->get_sector(flow_fields[i].first);
		auto sector_pos = sector->get_position().to_tile(sector_size);
		auto &flow_field = flow_fields[i].second;

		// navigate the flow field vectors until we reach its edge (or the target)
		flow_t cell;
		do {
			cell = flow_field->get_cell(current_x, current_y);

			if (cell & FLOW_LOS_MASK) {
				// check if we reached an LOS cell
				auto cell_pos = sector_pos + coord::tile_delta(current_x, current_y);
				waypoints.push_back(cell_pos);
				los_reached = true;
				break;
			}

			// check if we need to change direction
			auto cell_direction = static_cast<flow_dir_t>(cell & FLOW_DIR_MASK);
			if (cell_direction != current_direction) {
				// add the current cell as a waypoint
				auto cell_pos = sector_pos + coord::tile_delta(current_x, current_y);
				waypoints.push_back(cell_pos);
				current_direction = cell_direction;
			}

			// move to the next cell
			switch (current_direction) {
			case flow_dir_t::NORTH:
				current_y -= 1;
				break;
			case flow_dir_t::NORTH_EAST:
				current_x += 1;
				current_y -= 1;
				break;
			case flow_dir_t::EAST:
				current_x += 1;
				break;
			case flow_dir_t::SOUTH_EAST:
				current_x += 1;
				current_y += 1;
				break;
			case flow_dir_t::SOUTH:
				current_y += 1;
				break;
			case flow_dir_t::SOUTH_WEST:
				current_x -= 1;
				current_y += 1;
				break;
			case flow_dir_t::WEST:
				current_x -= 1;
				break;
			case flow_dir_t::NORTH_WEST:
				current_x -= 1;
				current_y -= 1;
				break;
			default:
				throw Error{ERR << "Invalid flow direction: " << static_cast<int>(current_direction)};
			}
		}
		while (not(cell & FLOW_TARGET_MASK));

		if (los_reached or i == flow_fields.size() - 1) {
			// exit the loop if we found an LOS cell or reached
			// the target cell in the last flow field
			break;
		}

		// reset the current position for the next flow field
		switch (current_direction) {
		case flow_dir_t::NORTH:
			current_y = sector_size - 1;
			break;
		case flow_dir_t::NORTH_EAST:
			current_x = current_x + 1;
			current_y = sector_size - 1;
			break;
		case flow_dir_t::EAST:
			current_x = 0;
			break;
		case flow_dir_t::SOUTH_EAST:
			current_x = 0;
			current_y = current_y + 1;
			break;
		case flow_dir_t::SOUTH:
			current_y = 0;
			break;
		case flow_dir_t::SOUTH_WEST:
			current_x = current_x - 1;
			current_y = 0;
			break;
		case flow_dir_t::WEST:
			current_x = sector_size - 1;
			break;
		case flow_dir_t::NORTH_WEST:
			current_x = sector_size - 1;
			current_y = current_y - 1;
			break;
		default:
			throw Error{ERR << "Invalid flow direction: " << static_cast<int>(current_direction)};
		}
	}

	// add the target position as the last waypoint
	waypoints.push_back(request.target);

	return waypoints;
}

int Pathfinder::heuristic_cost(const coord::tile &portal_pos,
                               const coord::tile &target_pos) {
	auto portal_phys_pos = portal_pos.to_phys2();
	auto target_phys_pos = target_pos.to_phys2();
	auto delta = target_phys_pos - portal_phys_pos;

	return delta.length();
}

int Pathfinder::distance_cost(const coord::tile_delta &portal1_pos,
                              const coord::tile_delta &portal2_pos) {
	auto delta = portal2_pos.to_phys2() - portal1_pos.to_phys2();

	return delta.length();
}


PortalNode::PortalNode(const std::shared_ptr<Portal> &portal) :
	portal{portal},
	entry_sector{NULL},
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

std::vector<node_t> PortalNode::generate_backtrace() {
	std::vector<node_t> waypoints;

	node_t current = this->shared_from_this();
	do {
		waypoints.push_back(current);
		current = current->prev_portal;
	}
	while (current != nullptr);

	return waypoints;
}

void PortalNode::init_exits(const nodemap_t &node_map) {
	auto exits = this->portal->get_exits(this->node_sector_0);
	for (auto &exit : exits) {
		int distance_cost = Pathfinder::distance_cost(
			this->portal->get_exit_center(this->node_sector_0),
			exit->get_entry_center(this->node_sector_1));

		auto exit_node = node_map.at(exit->get_id());
		this->exits_1[exit_node] = distance_cost;
	}

	exits = this->portal->get_exits(this->node_sector_1);
	for (auto &exit : exits) {
		int distance_cost = Pathfinder::distance_cost(
			this->portal->get_exit_center(this->node_sector_1),
			exit->get_entry_center(this->node_sector_0));

		auto exit_node = node_map.at(exit->get_id());
		this->exits_0[exit_node] = distance_cost;
	}
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


bool compare_node_cost::operator()(const node_t &lhs, const node_t &rhs) const {
	return *lhs < *rhs;
}

} // namespace openage::path
