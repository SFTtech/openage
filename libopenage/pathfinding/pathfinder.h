// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <map>
#include <memory>
#include <optional>
#include <unordered_map>

#include "coord/chunk.h"
#include "coord/phys.h"
#include "coord/tile.h"

#include "datastructure/pairing_heap.h"

#include "error/error.h"

#include "log/log.h"

#include "pathfinding/cost_field.h"
#include "pathfinding/definitions.h"
#include "pathfinding/flow_field.h"
#include "pathfinding/grid.h"
#include "pathfinding/integration_field.h"
#include "pathfinding/integrator.h"
#include "pathfinding/path.h"
#include "pathfinding/portal.h"
#include "pathfinding/sector.h"
#include "pathfinding/types.h"


namespace openage::path {

template <size_t SECTOR_SIDE_LENGTH>
class Grid;

template <size_t SECTOR_SIDE_LENGTH>
class Integrator;

template <size_t SECTOR_SIDE_LENGTH>
class FlowField;

class Portal;

/**
 * Pathfinder for flow field pathfinding.
 *
 * The pathfinder manages the grids defining the pathable ingame areas and
 * provides an interface for making pathfinding requests.
 *
 * Pathfinding consists of a multi-step process: First, there is a high-level
 * search using A* to identify the sectors of the grid that should be traversed.
 * Afterwards, flow fields are calculated from the target sector to the start
 * sector, which are then used to guide the actual unit movement.
 */

template <size_t SECTOR_SIDE_LENGTH>
class Pathfinder {
public:
	/**
	 * Create a new pathfinder.
	 */
	Pathfinder();
	~Pathfinder() = default;

	/**
	 * Get the grid at a specified index.
	 *
	 * @param id ID of the grid.
	 *
	 * @return Pathfinding grid.
	 */
	const std::shared_ptr<Grid<SECTOR_SIDE_LENGTH>> &get_grid(grid_id_t id) const;

	/**
	 * Add a grid to the pathfinder.
	 *
	 * @param grid Grid to add.
	 */
	void add_grid(const std::shared_ptr<Grid<SECTOR_SIDE_LENGTH>> &grid);

	/**
	 * Get the path for a pathfinding request.
	 *
	 * @param request Pathfinding request.
	 *
	 * @return Path found by the pathfinder.
	 */
	const Path get_path(const PathRequest &request);


	/**
	 * Calculate the distance cost between two portals.
	 *
	 * @param portal1_pos Center of the first portal (relative to sector origin).
	 * @param portal2_pos Center of the second portal (relative to sector origin).
	 *
	 * @return Distance cost between the portal centers.
	 */
	static int distance_cost(const coord::tile_delta &portal1_pos, const coord::tile_delta &portal2_pos);

private:
	using portal_star_t = std::pair<PathResult, std::vector<std::shared_ptr<Portal>>>;

	/**
	 * High-level pathfinder. Uses A* to find the path through the portals of sectors.
	 *
	 * @param request Pathfinding request.
	 * @param target_portal_ids IDs of portals that can be reached from the target cell.
	 * @param start_portal_ids IDs of portals that can be reached from the start cell.
	 *
	 * @return Portals to traverse in order to reach the target.
	 */
	const portal_star_t portal_a_star(const PathRequest &request,
	                                  const std::unordered_set<portal_id_t> &target_portal_ids,
	                                  const std::unordered_set<portal_id_t> &start_portal_ids) const;

	/**
	 * Low-level pathfinder. Uses flow fields to find the path through the sectors.
	 *
	 * @param flow_fields Flow fields for the sectors.
	 * @param request Pathfinding request.
	 *
	 * @return Waypoint coordinates to traverse in order to reach the target.
	 */
	const std::vector<coord::tile> get_waypoints(const std::vector<std::pair<sector_id_t, std::shared_ptr<FlowField<SECTOR_SIDE_LENGTH>>>> &flow_fields,
	                                             const PathRequest &request) const;

	/**
	 * Calculate the heuristic cost between a portal and a target cell.
	 *
	 * @param portal_pos Position of the portal (absolute on the grid).
	 *                   This should be the center of the portal exit.
	 * @param target_pos Position of the target cell (absolute on the grid).
	 *
	 * @return Heuristic cost between the cells.
	 */
	static int heuristic_cost(const coord::tile &portal_pos, const coord::tile &target_pos);


	/**
	 * Grids managed by this pathfinder.
	 *
	 * Each grid can have separate pathing.
	 */
	std::unordered_map<grid_id_t, std::shared_ptr<Grid<SECTOR_SIDE_LENGTH>>> grids;

	/**
	 * Integrator for flow field calculations.
	 */
	std::shared_ptr<Integrator<SECTOR_SIDE_LENGTH>> integrator;
};

class PortalNode;

using node_t = std::shared_ptr<PortalNode>;


using heap_t = datastructure::PairingHeap<node_t, PortalNode::compare_node_cost>;
using nodemap_t = std::unordered_map<portal_id_t, node_t>;

template <size_t SECTOR_SIDE_LENGTH>
Pathfinder<SECTOR_SIDE_LENGTH>::Pathfinder() :
	grids{},
	integrator{std::make_shared<Integrator<SECTOR_SIDE_LENGTH>>()} {}

template <size_t SECTOR_SIDE_LENGTH>
const Path Pathfinder<SECTOR_SIDE_LENGTH>::get_path(const PathRequest &request) {
	auto grid = this->grids.at(request.grid_id);

	// Check if the target is within the grid
	auto grid_size = grid->get_size();
	auto grid_width = grid_size[0] * SECTOR_SIDE_LENGTH;
	auto grid_height = grid_size[1] * SECTOR_SIDE_LENGTH;
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

	auto start_sector_x = request.start.ne / SECTOR_SIDE_LENGTH;
	auto start_sector_y = request.start.se / SECTOR_SIDE_LENGTH;
	auto start_sector = grid->get_sector(start_sector_x, start_sector_y);

	auto target_sector_x = request.target.ne / SECTOR_SIDE_LENGTH;
	auto target_sector_y = request.target.se / SECTOR_SIDE_LENGTH;
	auto target_sector = grid->get_sector(target_sector_x, target_sector_y);

	auto target = request.target - target_sector->get_position().to_tile(SECTOR_SIDE_LENGTH);
	if (target_sector->get_cost_field()->get_cost(target) == COST_IMPASSABLE) {
		// TODO: This may be okay if the target is a building or unit
		log::log(DBG << "Path not found (start = "
		             << request.start << "; target = "
		             << request.target << "): "
		             << "Target is impassable.");
		return Path{request.grid_id, PathResult::NOT_FOUND, {}};
	}

	// Integrate the target field
	coord::tile_delta target_delta = request.target - target_sector->get_position().to_tile(SECTOR_SIDE_LENGTH);
	auto target_integration_field = this->integrator->integrate(target_sector->get_cost_field(),
	                                                            target_delta);

	if (target_sector == start_sector) {
		auto start = request.start - start_sector->get_position().to_tile(SECTOR_SIDE_LENGTH);

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
	coord::tile_delta start = request.start - start_sector->get_position().to_tile(SECTOR_SIDE_LENGTH);
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

	typename Integrator<SECTOR_SIDE_LENGTH>::get_return_t sector_fields{prev_integration_field, prev_flow_field};

	std::vector<std::pair<sector_id_t, std::shared_ptr<FlowField<SECTOR_SIDE_LENGTH>>>> flow_fields;
	flow_fields.reserve(portal_path.size() + 1);
	flow_fields.push_back(std::make_pair(target_sector->get_id(), sector_fields.second));

	int los_depth = 1;

	for (auto &portal : portal_path) {
		auto prev_sector = grid->get_sector(prev_sector_id);
		auto next_sector_id = portal->get_exit_sector(prev_sector_id);
		auto next_sector = grid->get_sector(next_sector_id);

		target_delta = request.target - next_sector->get_position().to_tile(SECTOR_SIDE_LENGTH);
		bool with_los = los_depth > 0;

		sector_fields = this->integrator->get(next_sector->get_cost_field(),
		                                      prev_integration_field,
		                                      prev_sector_id,
		                                      portal,
		                                      target_delta,
		                                      request.time,
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

template <size_t SECTOR_SIDE_LENGTH>
const Pathfinder<SECTOR_SIDE_LENGTH>::portal_star_t Pathfinder<SECTOR_SIDE_LENGTH>::portal_a_star(const PathRequest &request,
                                                                                                  const std::unordered_set<portal_id_t> &target_portal_ids,
                                                                                                  const std::unordered_set<portal_id_t> &start_portal_ids) const {
	std::vector<std::shared_ptr<Portal>> result;

	auto grid = this->grids.at(request.grid_id);
	auto &portal_map = grid->get_portal_map();

	auto start_sector_x = request.start.ne / SECTOR_SIDE_LENGTH;
	auto start_sector_y = request.start.se / SECTOR_SIDE_LENGTH;
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

		auto sector_pos = grid->get_sector(portal->get_exit_sector(start_sector->get_id()))->get_position().to_tile(SECTOR_SIDE_LENGTH);
		auto portal_pos = portal->get_exit_center(start_sector->get_id());
		auto portal_abs_pos = sector_pos + portal_pos;
		auto heuristic_cost = Pathfinder<SECTOR_SIDE_LENGTH>::heuristic_cost(portal_abs_pos, request.target);
		portal_node->current_cost = Pathfinder<SECTOR_SIDE_LENGTH>::heuristic_cost(portal_abs_pos, request.start);
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
		ENSURE(current_node->entry_sector != std::nullopt, "Entry sector not set for portal node.");
		const auto &exits = current_node->get_exits(current_node->entry_sector.value());

		// evaluate all neighbors of the current candidate for further progress
		for (auto &[exit, distance_cost] : exits) {
			exit->entry_sector = current_node->portal->get_exit_sector(current_node->entry_sector.value());
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
					auto exit_sector = grid->get_sector(exit->portal->get_exit_sector(exit->entry_sector.value()));
					auto exit_sector_pos = exit_sector->get_position().to_tile(SECTOR_SIDE_LENGTH);
					auto exit_portal_pos = exit->portal->get_exit_center(exit->entry_sector.value());
					exit->heuristic_cost = Pathfinder<SECTOR_SIDE_LENGTH>::heuristic_cost(
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

template <size_t SECTOR_SIDE_LENGTH>
const std::vector<coord::tile> Pathfinder<SECTOR_SIDE_LENGTH>::get_waypoints(const std::vector<std::pair<sector_id_t, std::shared_ptr<FlowField<SECTOR_SIDE_LENGTH>>>> &flow_fields,
                                                                             const PathRequest &request) const {
	ENSURE(flow_fields.size() > 0, "At least 1 flow field is required for finding waypoints.");

	std::vector<coord::tile> waypoints;

	auto grid = this->get_grid(request.grid_id);
	coord::tile_t start_x = request.start.ne % SECTOR_SIDE_LENGTH;
	coord::tile_t start_y = request.start.se % SECTOR_SIDE_LENGTH;

	bool los_reached = false;

	coord::tile_t current_x = start_x;
	coord::tile_t current_y = start_y;
	flow_dir_t current_direction = flow_fields.at(0).second->get_dir(current_x, current_y);
	for (size_t i = 0; i < flow_fields.size(); ++i) {
		auto &sector = grid->get_sector(flow_fields[i].first);
		auto sector_pos = sector->get_position().to_tile(SECTOR_SIDE_LENGTH);
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
			current_y = SECTOR_SIDE_LENGTH - 1;
			break;
		case flow_dir_t::NORTH_EAST:
			current_x = current_x + 1;
			current_y = SECTOR_SIDE_LENGTH - 1;
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
			current_x = SECTOR_SIDE_LENGTH - 1;
			break;
		case flow_dir_t::NORTH_WEST:
			current_x = SECTOR_SIDE_LENGTH - 1;
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

template <size_t SECTOR_SIDE_LENGTH>
int Pathfinder<SECTOR_SIDE_LENGTH>::heuristic_cost(const coord::tile &portal_pos,
                                                   const coord::tile &target_pos) {
	auto portal_phys_pos = portal_pos.to_phys2();
	auto target_phys_pos = target_pos.to_phys2();
	auto delta = target_phys_pos - portal_phys_pos;

	return delta.length();
}

template <size_t SECTOR_SIDE_LENGTH>
const std::shared_ptr<Grid<SECTOR_SIDE_LENGTH>> &Pathfinder<SECTOR_SIDE_LENGTH>::get_grid(grid_id_t id) const {
	return this->grids.at(id);
}

template <size_t SECTOR_SIDE_LENGTH>
void Pathfinder<SECTOR_SIDE_LENGTH>::add_grid(const std::shared_ptr<Grid<SECTOR_SIDE_LENGTH>> &grid) {
	this->grids[grid->get_id()] = grid;
}


} // namespace openage::path
