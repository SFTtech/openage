// Copyright 2014-2019 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#include "../coord/phys.h"
#include "../coord/tile.h"
#include "../datastructure/pairing_heap.h"
#include "../util/misc.h"


namespace openage {

namespace coord {
class CoordManager;
}

namespace path {

class Node;
class Path;

/**
 * The data type for movement cost
 */
using cost_t = float;

/**
 * Type for storing navigation nodes.
 */
using node_pt = std::shared_ptr<Node>;

/**
 * Type for mapping tiles to nodes.
 */
using nodemap_t = std::unordered_map<coord::phys3, node_pt>;


/**
 * Cost comparison for node_pt.
 * Extracts the ptr from the shared_ptr.
 * Calls operator < on Node.
 */
struct compare_node_cost {
	bool operator ()(const node_pt &lhs, const node_pt &rhs) const;
};

/**
 * Priority queue node item type.
 */
using heap_t = datastructure::PairingHeap<node_pt, compare_node_cost>;

/**
 * Size of phys-coord grid for path nodes.
 */
constexpr coord::phys_t path_grid_size{1.f/8};

/**
 * Phys3 delta coordinates to select for path neighbors.
 */
constexpr coord::phys3_delta const neigh_phys[] = {
	{path_grid_size *  1, path_grid_size * -1, 0},
	{path_grid_size *  1, path_grid_size *  0, 0},
	{path_grid_size *  1, path_grid_size *  1, 0},
	{path_grid_size *  0, path_grid_size *  1, 0},
	{path_grid_size * -1, path_grid_size *  1, 0},
	{path_grid_size * -1, path_grid_size *  0, 0},
	{path_grid_size * -1, path_grid_size * -1, 0},
	{path_grid_size *  0, path_grid_size * -1, 0}
};

/**
 *
 */
bool passable_line(node_pt start, node_pt end, std::function<bool(const coord::phys3 &)>passable, float samples=5.0f);

/**
 * One navigation waypoint in a path.
 */
class Node: public std::enable_shared_from_this<Node> {
public:
	Node(const coord::phys3 &pos, node_pt prev);
	Node(const coord::phys3 &pos, node_pt prev, cost_t past, cost_t heuristic);

	/**
	 * Orders nodes according to their future cost value.
	 */
	bool operator <(const Node &other) const;

	/**
	 * Compare the node to another one.
	 * They are the same if their position is.
	 */
	bool operator ==(const Node &other) const;

	/**
	 * Calculates the actual movement cose to another node.
	 */
	cost_t cost_to(const Node &other) const;

	/**
	 * Create a backtrace path beginning at this node.
	 */
	Path generate_backtrace();

	/**
	 * Get all neighbors of this graph node.
	 */
	std::vector<node_pt> get_neighbors(const nodemap_t &, float scale=1.0f);

	/**
	 * The tile position this node is associated to.
	 * \todo make const
	 */
	coord::phys3 position;
	coord::tile tile_position;
	coord::phys3_delta direction; // for path smoothing

	/**
	 * Future cost estimation value for this node.
	 */
	cost_t future_cost;

	/**
	 * Evaluated past cost value for the node.
	 * This stores the actual cost from start to this node.
	 */
	cost_t past_cost;

	/**
	 * Heuristic cost cache.
	 * Calculated once, is the heuristic distance from this node
	 * to the goal.
	 */
	cost_t heuristic_cost;

	/**
	 * Can this node be passed?
	 */
	bool accessible = false;

	/**
	 * Has this Node been visited?
	 */
	bool visited = false;

	/**
	 * Does this node already have an alternative path?
	 * If the node was once selected as the best next hop,
	 * this is set to true.
	 */
	bool was_best = false;

	/**
	 * Factor to adjust movement cost.
	 * default: 1
	 */
	cost_t factor;

	/**
	 * Node where this one was reached by least cost.
	 */
	node_pt path_predecessor;

	/**
	 * Priority queue node that contains this path node.
	 */
	heap_t::element_t heap_node;
};


/**
 * Represents a planned trajectory.
 * Generated by pathfinding algorithms.
 */
class Path {
public:
	Path() = default;
	Path(const std::vector<Node> &nodes);

	void draw_path(const coord::CoordManager &mgr);

	/**
	 * These are the waypoints to navigate in order.
	 * Includes the start and end node.
	 */
	std::vector<Node> waypoints;
};

} // namespace path
} // namespace openage


namespace std {

/**
 * Hash function for path nodes.
 * Just uses their position.
 */
template<>
struct hash<openage::path::Node &> {
	size_t operator ()(const openage::path::Node &x) const {
		openage::coord::phys3 node_pos = x.position;
		size_t nehash = std::hash<openage::coord::phys_t>{}(node_pos.ne);
		size_t sehash = std::hash<openage::coord::phys_t>{}(node_pos.se);
		return openage::util::rol<size_t, 1>(nehash) ^ sehash;
	}
};

} // namespace std
