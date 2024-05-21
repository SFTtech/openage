// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "coord/tile.h"
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
	 * @param id ID of the portal. Should be unique per grid.
	 * @param sector0 First sector connected by the portal.
	 *                Must be north or east on the grid in relation to sector 1.
	 * @param sector1 Second sector connected by the portal.
	 *                Must be south or west on the grid in relation to sector 0.
	 * @param direction Direction of the portal from sector 0 to sector 1.
	 * @param cell_start Start cell coordinate in sector 0 (relative to sector origin).
	 * @param cell_end End cell coordinate in sector 0 (relative to sector origin).
	 */
	Portal(portal_id_t id,
	       sector_id_t sector0,
	       sector_id_t sector1,
	       PortalDirection direction,
	       const coord::tile_delta &cell_start,
	       const coord::tile_delta &cell_end);

	~Portal() = default;

	/**
	 * Get the ID of the portal.
	 *
	 * IDs are unique per grid.
	 *
	 * @return ID of the portal.
	 */
	portal_id_t get_id() const;

	/**
	 * Get the connected portals in the specified sector.
	 *
	 * @param sector Sector ID.
	 *
	 * @return Connected portals in the sector.
	 */
	const std::vector<std::shared_ptr<Portal>> &get_connected(sector_id_t sector) const;

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
	 * Get the cell coordinates of the start of the portal in the entry sector.
	 *
	 * @param entry_sector Sector from which the portal is entered.
	 *
	 * @return Cell coordinates of the start of the portal in the entry sector (relative to sector origin).
	 */
	const coord::tile_delta get_entry_start(sector_id_t entry_sector) const;

	/**
	 * Get the cell coordinates of the center of the portal in the entry sector.
	 *
	 * @param entry_sector Sector from which the portal is entered.
	 *
	 * @return Cell coordinates of the center of the portal in the entry sector (relative to sector origin).
	 */
	const coord::tile_delta get_entry_center(sector_id_t entry_sector) const;

	/**
	 * Get the cell coordinates of the start of the portal in the entry sector.
	 *
	 * @param entry_sector Sector from which the portal is entered.
	 *
	 * @return Cell coordinates of the start of the portal in the entry sector (relative to sector origin).
	 */
	const coord::tile_delta get_entry_end(sector_id_t entry_sector) const;

	/**
	 * Get the cell coordinates of the start of the portal in the exit sector.
	 *
	 * @param entry_sector Sector from which the portal is entered.
	 *
	 * @return Cell coordinates of the start of the portal in the exit sector (relative to sector origin).
	 */
	const coord::tile_delta get_exit_start(sector_id_t entry_sector) const;

	/**
	 * Get the cell coordinates of the center of the portal in the exit sector.
	 *
	 * @param entry_sector Sector from which the portal is entered.
	 *
	 * @return Cell coordinates of the center of the portal in the exit sector (relative to sector origin).
	 */
	const coord::tile_delta get_exit_center(sector_id_t entry_sector) const;

	/**
	 * Get the cell coordinates of the end of the portal in the exit sector.
	 *
	 * @param entry_sector Sector from which the portal is entered.
	 *
	 * @return Cell coordinates of the end of the portal in the exit sector (relative to sector origin).
	 */
	const coord::tile_delta get_exit_end(sector_id_t entry_sector) const;

	/**
	 * Get the direction of the portal from sector 0 to sector 1.
	 *
	 * @return Direction of the portal.
	 */
	PortalDirection get_direction() const;

private:
	/**
	 * Get the start cell coordinates of the portal.
	 *
	 * @return Start cell coordinates of the portal (relative to sector origin).
	 */
	const coord::tile_delta &get_sector0_start() const;

	/**
	 * Get the end cell coordinates of the portal.
	 *
	 * @return End cell coordinates of the portal (relative to sector origin).
	 */
	const coord::tile_delta &get_sector0_end() const;

	/**
	 * Get the start cell coordinates of the portal.
	 *
	 * @return Start cell coordinates of the portal (relative to sector origin).
	 */
	const coord::tile_delta get_sector1_start() const;

	/**
	 * Get the end cell coordinates of the portal.
	 *
	 * @return End cell coordinates of the portal (relative to sector origin).
	 */
	const coord::tile_delta get_sector1_end() const;

	/**
	 * ID of the portal.
	 */
	portal_id_t id;

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
	 *
	 * TODO: Also store avarage cost to reach each exit.
	 */
	std::vector<std::shared_ptr<Portal>> sector0_exits;

	/**
	 * Exits in sector 1 reachable from the portal.
	 *
	 * TODO: Also store avarage cost to reach each exit.
	 */
	std::vector<std::shared_ptr<Portal>> sector1_exits;

	/**
	 * Direction of the portal from sector 0 to sector 1.
	 */
	PortalDirection direction;

	/**
	 * Start cell coordinate in sector 0 (relative to sector origin).
	 *
	 * Coordinates for sector 1 are calculated on-the-fly using the direction.
	 */
	coord::tile_delta cell_start;

	/**
	 * End cell coordinate in sector 0 (relative to sector origin).
	 *
	 * Coordinates for sector 1 are calculated on-the-fly using the direction.
	 */
	coord::tile_delta cell_end;
};
} // namespace openage::path
