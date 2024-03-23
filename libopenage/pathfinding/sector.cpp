// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "sector.h"

#include "error/error.h"

#include "coord/tile.h"
#include "pathfinding/cost_field.h"
#include "pathfinding/definitions.h"


namespace openage::path {

Sector::Sector(sector_id_t id, size_t field_size) :
	id{id},
	cost_field{std::make_shared<CostField>(field_size)} {
}

Sector::Sector(sector_id_t id, const std::shared_ptr<CostField> &cost_field) :
	id{id},
	cost_field{cost_field} {
}

const sector_id_t &Sector::get_id() const {
	return this->id;
}

const std::shared_ptr<CostField> &Sector::get_cost_field() const {
	return this->cost_field;
}

const std::vector<std::shared_ptr<Portal>> &Sector::get_portals() const {
	return this->portals;
}

void Sector::add_portal(const std::shared_ptr<Portal> &portal) {
	this->portals.push_back(portal);
}

std::vector<std::shared_ptr<Portal>> Sector::find_portals(const std::shared_ptr<Sector> &other,
                                                          PortalDirection direction,
                                                          portal_id_t next_id) const {
	ENSURE(this->cost_field->get_size() == other->get_cost_field()->get_size(), "Sector size mismatch");

	std::vector<std::shared_ptr<Portal>> result;

	// cost field of the other sector
	auto other_cost = other->get_cost_field();

	// compare the edges of the sectors
	if (direction == PortalDirection::NORTH_SOUTH) {
		// search from left to right
		size_t start = 0;
		bool passable_edge = false;
		for (size_t x = 0; x < this->cost_field->get_size(); x++) {
			if (this->cost_field->get_cost(coord::tile{x, this->cost_field->get_size() - 1}) != COST_IMPASSABLE
			    and other_cost->get_cost(coord::tile{x, 0}) != COST_IMPASSABLE) {
				if (not passable_edge) {
					start = x;
					passable_edge = true;
				}
			}
			else {
				if (passable_edge) {
					result.push_back(
						std::make_shared<Portal>(
							next_id,
							this->id,
							other->get_id(),
							direction,
							coord::tile{start, this->cost_field->get_size() - 1},
							coord::tile{x - 1, 0}));
					passable_edge = false;
					next_id += 1;
				}
			}
		}
	}
	else if (direction == PortalDirection::EAST_WEST) {
		// search from top to bottom
		size_t start = 0;
		bool passable_edge = false;
		for (size_t y = 0; y < this->cost_field->get_size(); y++) {
			if (this->cost_field->get_cost(coord::tile{this->cost_field->get_size() - 1, y}) != COST_IMPASSABLE
			    and other_cost->get_cost(coord::tile{0, y}) != COST_IMPASSABLE) {
				if (not passable_edge) {
					start = y;
					passable_edge = true;
				}
			}
			else {
				if (passable_edge) {
					result.push_back(
						std::make_shared<Portal>(
							next_id,
							this->id,
							other->get_id(),
							direction,
							coord::tile{this->cost_field->get_size() - 1, start},
							coord::tile{0, y - 1}));
					passable_edge = false;
					next_id += 1;
				}
			}
		}
	}

	return result;
}

void Sector::connect_portals() {
	// TODO: Flood fill to find connected sectors
}

} // namespace openage::path
