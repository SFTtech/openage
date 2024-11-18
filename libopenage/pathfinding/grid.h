// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <memory>
#include <utility>
#include <vector>

#include "pathfinding/pathfinder.h"
#include "pathfinding/types.h"
#include "util/vector.h"


namespace openage::path {
class Sector;

/**
 * Grid for flow field pathfinding.
 */
class Grid {
public:
	/**
	 * Create a new empty grid of width x height sectors with a specified size.
	 *
	 * @param id ID of the grid.
	 * @param size Size of the grid in sectors (width x height).
	 * @param sector_size Side length of each sector.
	 */
	Grid(grid_id_t id,
	     const util::Vector2s &size,
	     size_t sector_size);

	/**
	 * Create a grid of width x height sectors from a list of existing sectors.
	 *
	 * @param id ID of the grid.
	 * @param size Size of the grid in sectors (width x height).
	 * @param sectors Existing sectors.
	 */
	Grid(grid_id_t id,
	     const util::Vector2s &size,
	     std::vector<std::shared_ptr<Sector>> &&sectors);

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
	 * Get the side length of the sectors on the grid (in number of cells).
	 *
	 * @return Sector side length (in number of cells).
	 */
	size_t get_sector_size() const;

	/**
	 * Get the sector at a specified position.
	 *
	 * @param x X coordinate.
	 * @param y Y coordinate.
	 *
	 * @return Sector at the specified position.
	 */
	const std::shared_ptr<Sector> &get_sector(size_t x, size_t y);

	/**
	 * Get the sector with a specified ID
	 *
	 * @param id ID of the sector.
	 *
	 * @return Sector with the specified ID.
	 */
	const std::shared_ptr<Sector> &get_sector(sector_id_t id) const;

	/**
	 * Get the sectors of the grid.
	 *
	 * @return Sectors of the grid.
	 */
	const std::vector<std::shared_ptr<Sector>> &get_sectors() const;

	/**
	 * Initialize the portals of the sectors on the grid.
	 *
	 * This should be called after all sectors' cost fields have been initialized.
	 */
	void init_portals();

	/**
	 * returns map of portal ids to portal nodes
	 */
	const nodemap_t &get_portal_map();

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
	 * Side length of the grid sectors.
	 */
	size_t sector_size;

	/**
	 * Sectors of the grid.
	 */
	std::vector<std::shared_ptr<Sector>> sectors;

	/**
	 *	maps portal_ids to portal nodes, which store their neigbouring nodes and associated distance costs
	 *  for pathfinding
	 */

	nodemap_t portal_nodes;
};


} // namespace openage::path
