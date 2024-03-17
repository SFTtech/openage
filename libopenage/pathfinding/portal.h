// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "pathfinding/types.h"


namespace openage::path {
class CostField;

/**
 * Possible directions of a portal node.
 */
enum class PortalDirection {
	NORTH_SOUTH,
	EAST_WEST
};

/**
 * Biderectional gateway for connecting two sectors in the flow field pathfinder.
 *
 * Portals are located at the border of two sectors (0 and 1), and allow units to move between them.
 * For each of these sectors, the portal stores the start and end coordinates where the
 * sectors overlap as well as the other portals that can be reached in the same
 * sector. For simplicity, the portal is assumed to be a straight line of cells from the start
 * to the end.
 *
 * The portal is bidirectional, meaning that it can be entered from either sector and
 * exited into the other sector. The direction of the portal from one sector to the other
 * is stored in the portal node. As a convention and to simplify computations, sector 0 must be
 * the either the north or east sector on the grid in relation to sector 1.
 */
class Portal {
public:
	/**
	 * Create a new portal between two sectors.
	 *
	 * As a convention, sector 0 must be the either the north or east sector
	 * on the grid in relation to sector 1.
	 *
	 * @param sector0 First sector connected by the portal.
	 *                Must be north or east on the grid in relation to sector 1.
	 * @param sector1 Second sector connected by the portal.
	 *                Must be south or west on the grid in relation to sector 0.
	 * @param direction Direction of the portal from sector 0 to sector 1.
	 * @param cell_start_x Start cell x coordinate in sector 0.
	 * @param cell_start_y Start cell y coordinate in sector 0.
	 * @param cell_end_x End cell x coordinate in sector 0.
	 * @param cell_end_y End cell y coordinate in sector 0.
	 */
	Portal(sector_id_t sector0,
	       sector_id_t sector1,
	       PortalDirection direction,
	       size_t cell_start_x,
	       size_t cell_start_y,
	       size_t cell_end_x,
	       size_t cell_end_y);

	~Portal() = default;

	/**
	 * Get the exit portals reachable via the portal when entering from a specified sector.
	 *
	 * @param entry_sector Sector from which the portal is entered.
	 *
	 * @return Exit portals reachable from the portal.
	 */
	const std::vector<std::shared_ptr<Portal>> &get_exits(sector_id_t entry_sector) const;

	/**
	 * Set the exit portals reachable for a specified sector.
	 *
	 * @param sector Sector for which the exit portals are set.
	 * @param exits Exit portals reachable from the portal.
	 */
	void set_exits(sector_id_t sector, const std::vector<std::shared_ptr<Portal>> &exits);

	/**
	 * Get the cost field of the sector where the portal is exited.
	 *
	 * @param entry_sector Sector from which the portal is entered.
	 *
	 * @return Cost field of the sector where the portal is exited.
	 */
	sector_id_t get_exit_sector(sector_id_t entry_sector) const;

	/**
	 * Get the cost field of the sector from which the portal is entered.
	 *
	 * @param entry_sector Sector from which the portal is entered.
	 *
	 * @return Cost field of the sector from which the portal is entered.
	 */
	std::pair<size_t, size_t> get_entry_start(sector_id_t entry_sector) const;

	/**
	 * Get the cell coordinates of the start of the portal in the entry sector.
	 *
	 * @param entry_sector Sector from which the portal is entered.
	 *
	 * @return Cell coordinates of the start of the portal in the entry sector.
	 */
	std::pair<size_t, size_t> get_entry_end(sector_id_t entry_sector) const;

	/**
	 * Get the cell coordinates of the start of the portal in the exit sector.
	 *
	 * @param entry_sector Sector from which the portal is entered.
	 *
	 * @return Cell coordinates of the start of the portal in the exit sector.
	 */
	std::pair<size_t, size_t> get_exit_start(sector_id_t entry_sector) const;

	/**
	 * Get the cell coordinates of the end of the portal in the exit sector.
	 *
	 * @param entry_sector Sector from which the portal is entered.
	 *
	 * @return Cell coordinates of the end of the portal in the exit sector.
	 */
	std::pair<size_t, size_t> get_exit_end(sector_id_t entry_sector) const;

private:
	/**
	 * Get the start cell coordinates of the portal.
	 *
	 * @return Start cell coordinates of the portal.
	 */
	std::pair<size_t, size_t> get_sector0_start() const;

	/**
	 * Get the end cell coordinates of the portal.
	 *
	 * @return End cell coordinates of the portal.
	 */
	std::pair<size_t, size_t> get_sector0_end() const;

	/**
	 * Get the start cell coordinates of the portal.
	 *
	 * @return Start cell coordinates of the portal.
	 */
	std::pair<size_t, size_t> get_sector1_start() const;

	/**
	 * Get the end cell coordinates of the portal.
	 *
	 * @return End cell coordinates of the portal.
	 */
	std::pair<size_t, size_t> get_sector1_end() const;

	/**
	 * First sector connected by the portal.
	 */
	sector_id_t sector0;

	/**
	 * Second sector connected by the portal.
	 */
	sector_id_t sector1;

	/**
	 * Exits in sector 0 reachable from the portal.
	 */
	std::vector<std::shared_ptr<Portal>> sector0_exits;

	/**
	 * Exits in sector 1 reachable from the portal.
	 */
	std::vector<std::shared_ptr<Portal>> sector1_exits;

	/**
	 * Direction of the portal from sector 0 to sector 1.
	 */
	PortalDirection direction;

	/**
	 * Start cell x coordinate.
	 */
	size_t cell_start_x;

	/**
	 * Start cell y coordinate.
	 */
	size_t cell_start_y;

	/**
	 * End cell x coordinate.
	 */
	size_t cell_end_x;

	/**
	 * End cell y coordinate.
	 */
	size_t cell_end_y;
};
} // namespace openage::path
