// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <memory>
#include <utility>
#include <vector>

#include "pathfinding/portal_node.h"
#include "pathfinding/types.h"

#include "util/vector.h"


namespace openage::path {

template <size_t SECTOR_SIDE_LENGTH>
class Sector;

/**
 * Grid for flow field pathfinding.
 */
template <size_t SECTOR_SIDE_LENGTH>
class Grid {
public:
	/**
	 * Create a new empty grid of width x height sectors with a specified size.
	 *
	 * @param id ID of the grid.
	 * @param size Size of the grid in sectors (width x height).
	 */
	Grid(grid_id_t id,
	     const util::Vector2s &size);

	/**
	 * Create a grid of width x height sectors from a list of existing sectors.
	 *
	 * @param id ID of the grid.
	 * @param size Size of the grid in sectors (width x height).
	 * @param sectors Existing sectors.
	 */
	Grid(grid_id_t id,
	     const util::Vector2s &size,
	     std::vector<std::shared_ptr<Sector<SECTOR_SIDE_LENGTH>>> &&sectors);

	/**
	 * Get the ID of the grid.
	 *
	 * @return ID of the grid.
	 */
	grid_id_t get_id() const;

	/**
	 * Get the size of the grid (in number of sectors).
	 *
	 * @return Size of the grid (in number of sectors) (width x height).
	 */
	const util::Vector2s &get_size() const;


	/**
	 * Get the sector at a specified position.
	 *
	 * @param x X coordinate.
	 * @param y Y coordinate.
	 *
	 * @return Sector at the specified position.
	 */
	const std::shared_ptr<Sector<SECTOR_SIDE_LENGTH>> &get_sector(size_t x, size_t y);

	/**
	 * Get the sector with a specified ID
	 *
	 * @param id ID of the sector.
	 *
	 * @return Sector with the specified ID.
	 */
	const std::shared_ptr<Sector<SECTOR_SIDE_LENGTH>> &get_sector(sector_id_t id) const;

	/**
	 * Get the sectors of the grid.
	 *
	 * @return Sectors of the grid.
	 */
	const std::vector<std::shared_ptr<Sector<SECTOR_SIDE_LENGTH>>> &get_sectors() const;

	/**
	 * Initialize the portals of the sectors on the grid.
	 *
	 * This should be called after all sectors' cost fields have been initialized.
	 */
	void init_portals();

	/**
	 * returns map of portal ids to portal nodes
	 */
	const PortalNode::nodemap_t &get_portal_map();

	/**
	 * Initialize the portal nodes of the grid with neigbouring nodes and distance costs.
	 */
	void init_portal_nodes();

private:
	/**
	 * ID of the grid.
	 */
	grid_id_t id;

	/**
	 * Size of the grid (width x height).
	 */
	util::Vector2s size;


	/**
	 * Sectors of the grid.
	 */
	std::vector<std::shared_ptr<Sector<SECTOR_SIDE_LENGTH>>> sectors;

	/**
	 *	maps portal_ids to portal nodes, which store their neigbouring nodes and associated distance costs
	 *  for pathfinding
	 */

	PortalNode::nodemap_t portal_nodes;
};

template <size_t SECTOR_SIDE_LENGTH>
Grid<SECTOR_SIDE_LENGTH>::Grid(grid_id_t id,
                               const util::Vector2s &size) :
	id{id},
	size{size} {
	for (size_t y = 0; y < size[1]; y++) {
		for (size_t x = 0; x < size[0]; x++) {
			this->sectors.push_back(
				std::make_shared<Sector<SECTOR_SIDE_LENGTH>>(x + y * this->size[0],
			                                                 coord::chunk(x, y)));
		}
	}
}

template <size_t SECTOR_SIDE_LENGTH>
Grid<SECTOR_SIDE_LENGTH>::Grid(grid_id_t id,
                               const util::Vector2s &size,
                               std::vector<std::shared_ptr<Sector<SECTOR_SIDE_LENGTH>>> &&sectors) :
	id{id},
	size{size},
	sectors{std::move(sectors)} {
	ENSURE(this->sectors.size() == size[0] * size[1],
	       "Grid has size " << size[0] << "x" << size[1] << " (" << size[0] * size[1] << " sectors), "
	                        << "but only " << this->sectors.size() << " sectors were provided");
}

template <size_t SECTOR_SIDE_LENGTH>
grid_id_t Grid<SECTOR_SIDE_LENGTH>::get_id() const {
	return this->id;
}

template <size_t SECTOR_SIDE_LENGTH>
const util::Vector2s &Grid<SECTOR_SIDE_LENGTH>::get_size() const {
	return this->size;
}

template <size_t SECTOR_SIDE_LENGTH>
const std::shared_ptr<Sector<SECTOR_SIDE_LENGTH>> &Grid<SECTOR_SIDE_LENGTH>::get_sector(size_t x, size_t y) {
	return this->sectors.at(x + y * this->size[0]);
}

template <size_t SECTOR_SIDE_LENGTH>
const std::shared_ptr<Sector<SECTOR_SIDE_LENGTH>> &Grid<SECTOR_SIDE_LENGTH>::get_sector(sector_id_t id) const {
	return this->sectors.at(id);
}

template <size_t SECTOR_SIDE_LENGTH>
const std::vector<std::shared_ptr<Sector<SECTOR_SIDE_LENGTH>>> &Grid<SECTOR_SIDE_LENGTH>::get_sectors() const {
	return this->sectors;
}

template <size_t SECTOR_SIDE_LENGTH>
void Grid<SECTOR_SIDE_LENGTH>::init_portals() {
	// Create portals between neighboring sectors.
	portal_id_t portal_id = 0;
	for (size_t y = 0; y < this->size[1]; y++) {
		for (size_t x = 0; x < this->size[0]; x++) {
			auto sector = this->get_sector(x, y);

			if (x < this->size[0] - 1) {
				auto neighbor = this->get_sector(x + 1, y);
				auto portals = sector->find_portals(neighbor, PortalDirection::EAST_WEST, portal_id);
				for (auto &portal : portals) {
					sector->add_portal(portal);
					neighbor->add_portal(portal);
				}
				portal_id += portals.size();
			}
			if (y < this->size[1] - 1) {
				auto neighbor = this->get_sector(x, y + 1);
				auto portals = sector->find_portals(neighbor, PortalDirection::NORTH_SOUTH, portal_id);
				for (auto &portal : portals) {
					sector->add_portal(portal);
					neighbor->add_portal(portal);
				}
				portal_id += portals.size();
			}
		}
	}

	// Connect mutually reachable exits of sectors.
	for (auto &sector : this->sectors) {
		sector->connect_exits();
	}
}

template <size_t SECTOR_SIDE_LENGTH>
const PortalNode::nodemap_t &Grid<SECTOR_SIDE_LENGTH>::get_portal_map() {
	return portal_nodes;
}


template <size_t SECTOR_SIDE_LENGTH>
void Grid<SECTOR_SIDE_LENGTH>::init_portal_nodes() {
	// create portal_nodes
	for (auto &sector : this->sectors) {
		for (auto &portal : sector->get_portals()) {
			if (!this->portal_nodes.contains(portal->get_id())) {
				auto portal_node = std::make_shared<PortalNode>(portal);
				portal_node->node_sector_0 = sector->get_id();
				portal_node->node_sector_1 = portal_node->portal->get_exit_sector(sector->get_id());
				this->portal_nodes[portal->get_id()] = portal_node;
			}
		}
	}

	// init portal_node exits
	for (auto &[id, node] : this->portal_nodes) {
		node->init_exits<SECTOR_SIDE_LENGTH>(this->portal_nodes);
	}
}


} // namespace openage::path
