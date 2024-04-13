// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <unordered_map>

#include "coord/tile.h"
#include "datastructure/pairing_heap.h"
#include "pathfinding/path.h"
#include "pathfinding/types.h"


namespace openage::path {
class Grid;
class Integrator;
class Portal;
class FlowField;

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
	const std::shared_ptr<Grid> &get_grid(grid_id_t id) const;

	/**
	 * Add a grid to the pathfinder.
	 *
	 * @param grid Grid to add.
	 */
	void add_grid(const std::shared_ptr<Grid> &grid);

	/**
	 * Get the path for a pathfinding request.
	 *
	 * @param request Pathfinding request.
	 *
	 * @return Path found by the pathfinder.
	 */
	const Path get_path(const PathRequest &request);

private:
	/**
	 * High-level pathfinder. Uses A* to find the path through the portals of sectors.
	 *
	 * @param request Pathfinding request.
	 *
	 * @return Portals to traverse in order to reach the target.
	 */
	const std::vector<std::shared_ptr<Portal>> portal_a_star(const PathRequest &request) const;

	/**
	 * Low-level pathfinder. Uses flow fields to find the path through the sectors.
	 *
	 * @param flow_fields Flow fields for the sectors.
	 * @param request Pathfinding request.
	 *
	 * @return Waypoint coordinates to traverse in order to reach the target.
	 */
	const std::vector<coord::tile> get_waypoints(const std::vector<std::pair<sector_id_t, std::shared_ptr<FlowField>>> &flow_fields,
	                                             const PathRequest &request) const;

	/**
	 * Calculate the heuristic cost between a portal and a target cell.
	 *
	 * @param portal_pos Position of the portal. This should be the center of the portal exit.
	 * @param target_pos Position of the target cell.
	 *
	 * @return Heuristic cost between the cells.
	 */
	static int heuristic_cost(const coord::tile &portal_pos, const coord::tile &target_pos);

	/**
	 * Grids managed by this pathfinder.
	 *
	 * Each grid can have separate pathing.
	 */
	std::unordered_map<grid_id_t, std::shared_ptr<Grid>> grids;

	/**
	 * Integrator for flow field calculations.
	 */
	std::shared_ptr<Integrator> integrator;
};


class PortalNode;

using node_t = std::shared_ptr<PortalNode>;

/**
 * Cost comparison for node_t on the pairing heap.
 *
 * Extracts the nodes from the shared_ptr and compares them. We have
 * to use a custom comparison function because otherwise the shared_ptr
 * would be compared instead of the actual node.
 */
struct compare_node_cost {
	bool operator()(const node_t &lhs, const node_t &rhs) const;
};

using heap_t = datastructure::PairingHeap<node_t, compare_node_cost>;
using nodemap_t = std::unordered_map<portal_id_t, node_t>;

/**
 * One navigation waypoint in a path.
 */
class PortalNode : public std::enable_shared_from_this<PortalNode> {
public:
	PortalNode(const std::shared_ptr<Portal> &portal,
	           sector_id_t entry_sector,
	           const node_t &prev_portal);
	PortalNode(const std::shared_ptr<Portal> &portal,
	           sector_id_t entry_sector,
	           const node_t &prev_portal,
	           int past_cost,
	           int heuristic_cost);

	/**
	 * Orders nodes according to their future cost value.
	 */
	bool operator<(const PortalNode &other) const;

	/**
	 * Compare the node to another one.
	 * They are the same if their portal is.
	 */
	bool operator==(const PortalNode &other) const;

	/**
	 * Calculates the actual movement cose to another node.
	 */
	int cost_to(const PortalNode &other) const;

	/**
	 * Create a backtrace path beginning at this node.
	 */
	std::vector<node_t> generate_backtrace();

	/**
	 * Get all exits of a node.
	 */
	std::vector<node_t> get_exits(const nodemap_t &nodes, sector_id_t entry_sector);

	/**
	 * The portal this node is associated to.
	 */
	std::shared_ptr<Portal> portal;

	/**
	 * Sector where the portal is entered.
	 */
	sector_id_t entry_sector;

	/**
	 * Future cost estimation value for this node.
	 */
	int future_cost;

	/**
	 * Evaluated past cost value for the node.
	 * This stores the actual cost from start to this node.
	 */
	int current_cost;

	/**
	 * Heuristic cost cache.
	 * Calculated once, is the heuristic distance from this node
	 * to the goal.
	 */
	int heuristic_cost;

	/**
	 * Does this node already have an alternative path?
	 * If the node was once selected as the best next hop,
	 * this is set to true.
	 */
	bool was_best = false;

	/**
	 * Node where this one was reached by least cost.
	 */
	node_t prev_portal;

	/**
	 * Priority queue node that contains this path node.
	 */
	heap_t::element_t heap_node;
};


} // namespace openage::path
