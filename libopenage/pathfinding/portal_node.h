// Copyright 2025-2025 the openage authors. See copying.md for legal info.

#pragma once


#include <memory>
#include <map>
#include <optional>

#include "pathfinding/portal.h"
#include "pathfinding/types.h"

#include "coord/tile.h"
#include "datastructure/pairing_heap.h"

namespace openage::path {

class Portal;

class PortalNode;

/**
 * One navigation waypoint in a path.
 */
class PortalNode : public std::enable_shared_from_this<PortalNode> {
public:

	using node_t = std::shared_ptr<PortalNode>;

	/**
	 * Cost comparison for node_t on the pairing heap.
	 *
	 * Extracts the nodes from the shared_ptr and compares them. We have
	 * to use a custom comparison function because otherwise the shared_ptr
	 * would be compared instead of the actual node.
	 */
	struct compare_node_cost {
		bool operator()(const PortalNode::node_t &lhs, const PortalNode::node_t &rhs) const;
	};

	using heap_t = datastructure::PairingHeap<node_t, struct compare_node_cost>;

	using nodemap_t = std::unordered_map<portal_id_t, node_t>;

	PortalNode(const std::shared_ptr<Portal> &portal);
	
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
	 * init PortalNode::exits.
	 */
	template <size_t SECTOR_SIDE_LENGTH>
	void init_exits(const nodemap_t &node_map);


	/**
	 * maps node_t of a neigbhour portal to the distance cost to travel between the portals
	 */
	using exits_t = std::map<const node_t, int>;


	/**
	 * Get the exit portals reachable via the portal when entering from a specified sector.
	 *
	 * @param entry_sector Sector from which the portal is entered.
	 *
	 * @return Exit portals nodes reachable from the portal.
	 */
	const exits_t &get_exits(sector_id_t entry_sector);

	/**
	 * The portal this node is associated to.
	 */
	std::shared_ptr<Portal> portal;

	/**
	 * Sector where the portal is entered.
	 */
	std::optional<sector_id_t> entry_sector;

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

	/**
	 * First sector connected by the portal.
	 */
	sector_id_t node_sector_0;

	/**
	 * Second sector connected by the portal.
	 */
	sector_id_t node_sector_1;

	/**
	 * Exits in sector 0 reachable from the portal.
	 */
	exits_t exits_0;

	/**
	 * Exits in sector 1 reachable from the portal.
	 */
	exits_t exits_1;
};

int get_distance_cost(const coord::tile_delta &portal1_pos, const coord::tile_delta &portal2_pos);

template <size_t SECTOR_SIDE_LENGTH>
void PortalNode::init_exits(const nodemap_t &node_map) {
	auto exits = this->portal->get_exits(this->node_sector_0);
	for (auto &exit : exits) {
		int distance_cost = get_distance_cost(
			this->portal->get_exit_center(this->node_sector_0),
			exit->get_entry_center(this->node_sector_1));

		auto exit_node = node_map.at(exit->get_id());
		this->exits_1[exit_node] = distance_cost;
	}

	exits = this->portal->get_exits(this->node_sector_1);
	for (auto &exit : exits) {
		int distance_cost = get_distance_cost(
			this->portal->get_exit_center(this->node_sector_1),
			exit->get_entry_center(this->node_sector_0));

		auto exit_node = node_map.at(exit->get_id());
		this->exits_0[exit_node] = distance_cost;
	}
}

} // namespace openage::path