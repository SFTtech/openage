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
	size_t start;
	bool passable_edge;
	for (size_t i = 0; i < this->cost_field->get_size(); ++i) {
		auto coord_this = coord::tile{0, 0};
		auto coord_other = coord::tile{0, 0};
		if (direction == PortalDirection::NORTH_SOUTH) {
			// right edge; top to bottom
			coord_this = coord::tile{i, this->cost_field->get_size() - 1};
			coord_other = coord::tile{i, 0};
		}
		else if (direction == PortalDirection::EAST_WEST) {
			// bottom edge; east to west
			coord_this = coord::tile{this->cost_field->get_size() - 1, i};
			coord_other = coord::tile{0, i};
		}

		if (this->cost_field->get_cost(coord_this) != COST_IMPASSABLE
		    and other_cost->get_cost(coord_other) != COST_IMPASSABLE) {
			if (not passable_edge) {
				start = i;
				passable_edge = true;
			}
		}
		else {
			if (passable_edge) {
				auto coord_start = coord::tile{0, 0};
				auto coord_end = coord::tile{0, 0};
				if (direction == PortalDirection::NORTH_SOUTH) {
					// right edge; top to bottom
					coord_start = coord::tile{start, this->cost_field->get_size() - 1};
					coord_end = coord::tile{i - 1, this->cost_field->get_size() - 1};
				}
				else if (direction == PortalDirection::EAST_WEST) {
					// bottom edge; east to west
					coord_start = coord::tile{this->cost_field->get_size() - 1, start};
					coord_end = coord::tile{this->cost_field->get_size() - 1, i - 1};
				}

				result.push_back(
					std::make_shared<Portal>(
						next_id,
						this->id,
						other->get_id(),
						direction,
						coord_start,
						coord_end));
				passable_edge = false;
				next_id += 1;
			}
		}
	}

	// recheck for the last tile on the edge
	// because it may be the end of a portal
	if (passable_edge) {
		auto coord_start = coord::tile{0, 0};
		auto coord_end = coord::tile{0, 0};
		if (direction == PortalDirection::NORTH_SOUTH) {
			coord_start = coord::tile{start, this->cost_field->get_size() - 1};
			coord_end = coord::tile{this->cost_field->get_size() - 1, this->cost_field->get_size() - 1};
		}
		else if (direction == PortalDirection::EAST_WEST) {
			coord_start = coord::tile{this->cost_field->get_size() - 1, start};
			coord_end = coord::tile{this->cost_field->get_size() - 1, this->cost_field->get_size() - 1};
		}

		result.push_back(
			std::make_shared<Portal>(
				next_id,
				this->id,
				other->get_id(),
				direction,
				coord_start,
				coord_end));
	}

	return result;
}

void Sector::connect_portals() {
	// TODO: Flood fill to find connected sectors
}

} // namespace openage::path
