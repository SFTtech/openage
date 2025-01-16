// Copyright 2024-2025 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "coord/chunk.h"
#include "pathfinding/portal.h"
#include "pathfinding/types.h"


namespace openage::path {
class CostField;
class Portal;

/**
 * Sector in a grid for flow field pathfinding.
 *
 * Sectors consist of a cost field and a list of portals connecting them to adjacent
 * sectors.
 */
class Sector {
public:
	/**
	 * Create a new sector with a specified ID and an uninitialized cost field.
	 *
	 * @param id ID of the sector. Should be unique per grid.
	 * @param position Position of the sector in the grid.
	 * @param field_size Size of the cost field.
	 */
	Sector(sector_id_t id,
	       const coord::chunk &position,
	       size_t field_size);

	/**
	 * Create a new sector with a specified ID and an existing cost field.
	 *
	 * @param id ID of the sector. Should be unique per grid.
	 * @param position Position of the sector in the grid.
	 * @param cost_field Cost field of the sector.
	 */
	Sector(sector_id_t id,
	       const coord::chunk &position,
	       const std::shared_ptr<CostField> &cost_field);

	/**
	 * Get the ID of this sector.
	 *
	 * IDs are unique per grid.
	 *
	 * @return ID of the sector.
	 */
	const sector_id_t &get_id() const;

	/**
	 * Get the position of this sector in the grid.
	 *
	 * @return Position of the sector (absolute on the grid).
	 */
	const coord::chunk &get_position() const;

	/**
	 * Get the cost field of this sector.
	 *
	 * @return Cost field of this sector.
	 */
	const std::shared_ptr<CostField> &get_cost_field() const;

	/**
	 * Get the portals connecting this sector to other sectors.
	 *
	 * @return Outgoing portals of this sector.
	 */
	const std::vector<std::shared_ptr<Portal>> &get_portals() const;

	/**
	 * Add a portal to another sector.
	 *
	 * @param portal Portal to another sector.
	 */
	void add_portal(const std::shared_ptr<Portal> &portal);

	/**
	 * Find portals connecting this sector to another sector.
	 *
	 * @param other Sector to which the portals should connect.
	 * @param direction Direction from this sector to \p other sector.
	 * @param next_id ID of the next portal to be created. Should be unique per grid.
	 *
	 * @return Portals connecting this sector to \p other sector.
	 */
	std::vector<std::shared_ptr<Portal>> find_portals(const std::shared_ptr<Sector> &other,
	                                                  PortalDirection direction,
	                                                  portal_id_t next_id) const;

	/**
	 * Connect all portals that are mutually reachable.
	 *
	 * This method should be called after all sectors and portals have
	 * been created and initialized.
	 */
	void connect_exits();

private:
	/**
	 * ID of the sector.
	 */
	sector_id_t id;

	/**
	 * Position of the sector (absolute on the grid).
	 */
	coord::chunk position;

	/**
	 * Cost field of the sector.
	 */
	std::shared_ptr<CostField> cost_field;

	/**
	 * Portals of the sector.
	 */
	std::vector<std::shared_ptr<Portal>> portals;
};


} // namespace openage::path
