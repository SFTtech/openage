// Copyright 2024-2025 the openage authors. See copying.md for legal info.

#include "sector.h"

#include <deque>
#include <unordered_set>

#include "error/error.h"

#include "coord/tile.h"
#include "pathfinding/cost_field.h"
#include "pathfinding/definitions.h"


namespace openage::path {

Sector::Sector(sector_id_t id, const coord::chunk &position, size_t field_size) :
	id{id},
	position{position},
	cost_field{std::make_shared<CostField>(field_size)} {
}

Sector::Sector(sector_id_t id, const coord::chunk &position, const std::shared_ptr<CostField> &cost_field) :
	id{id},
	position{position},
	cost_field{cost_field} {
}

const sector_id_t &Sector::get_id() const {
	return this->id;
}

const coord::chunk &Sector::get_position() const {
	return this->position;
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
	size_t start = 0;
	size_t end = 0;
	bool passable_edge = false;
	for (size_t i = 0; i < this->cost_field->get_size(); ++i) {
		auto coord_this = coord::tile_delta{0, 0};
		auto coord_other = coord::tile_delta{0, 0};
		if (direction == PortalDirection::NORTH_SOUTH) {
			// right edge; top to bottom
			coord_this = coord::tile_delta(i, this->cost_field->get_size() - 1);
			coord_other = coord::tile_delta(i, 0);
		}
		else if (direction == PortalDirection::EAST_WEST) {
			// bottom edge; east to west
			coord_this = coord::tile_delta(this->cost_field->get_size() - 1, i);
			coord_other = coord::tile_delta(0, i);
		}

		if (this->cost_field->get_cost(coord_this) != COST_IMPASSABLE
		    and other_cost->get_cost(coord_other) != COST_IMPASSABLE) {
			// both sides of the edge are passable
			if (not passable_edge) {
				// start a new portal
				start = i;
				passable_edge = true;
			}
			// else: we already started a portal

			end = i;
			if (i != this->cost_field->get_size() - 1) {
				// continue to next tile unless we are at the last tile
				// then we have to end the current portal
				continue;
			}
		}

		if (passable_edge) {
			// create a new portal
			auto coord_start = coord::tile_delta{0, 0};
			auto coord_end = coord::tile_delta{0, 0};
			if (direction == PortalDirection::NORTH_SOUTH) {
				// right edge; top to bottom
				coord_start = coord::tile_delta(start, this->cost_field->get_size() - 1);
				coord_end = coord::tile_delta(end, this->cost_field->get_size() - 1);
			}
			else if (direction == PortalDirection::EAST_WEST) {
				// bottom edge; east to west
				coord_start = coord::tile_delta(this->cost_field->get_size() - 1, start);
				coord_end = coord::tile_delta(this->cost_field->get_size() - 1, end);
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

	return result;
}

void Sector::connect_exits() {
	if (this->portals.empty()) {
		return;
	}

	std::unordered_set<portal_id_t> portal_ids;
	for (const auto &portal : this->portals) {
		portal_ids.insert(portal->get_id());
	}

	// check all portals in the sector
	std::vector<std::shared_ptr<Portal>> search_portals = this->portals;
	while (not portal_ids.empty()) {
		auto portal = search_portals.back();
		search_portals.pop_back();
		portal_ids.erase(portal->get_id());

		auto start = portal->get_entry_start(this->id);
		auto end = portal->get_entry_end(this->id);

		std::unordered_set<size_t> visited;
		std::deque<size_t> open_list;
		std::vector<size_t> neighbors;
		neighbors.reserve(4);

		if (portal->get_direction() == PortalDirection::NORTH_SOUTH) {
			// right edge; top to bottom
			for (auto i = start.se; i <= end.se; ++i) {
				open_list.push_back(start.ne + i * this->cost_field->get_size());
			}
		}
		else if (portal->get_direction() == PortalDirection::EAST_WEST) {
			// bottom edge; east to west
			for (auto i = start.ne; i <= end.ne; ++i) {
				open_list.push_back(i + start.se * this->cost_field->get_size());
			}
		}

		// flood fill the grid to find connected portals
		while (not open_list.empty()) {
			auto current = open_list.front();
			open_list.pop_front();

			if (visited.contains(current)) {
				continue;
			}

			// Get the x and y coordinates of the current cell
			auto x = current % this->cost_field->get_size();
			auto y = current / this->cost_field->get_size();

			// check the neighbors
			if (y > 0) {
				neighbors.push_back(current - this->cost_field->get_size());
			}
			if (x > 0) {
				neighbors.push_back(current - 1);
			}
			if (y < this->cost_field->get_size() - 1) {
				neighbors.push_back(current + this->cost_field->get_size());
			}
			if (x < this->cost_field->get_size() - 1) {
				neighbors.push_back(current + 1);
			}

			// add the neighbors to the open list
			for (const auto &neighbor : neighbors) {
				if (this->cost_field->get_cost(neighbor) != COST_IMPASSABLE) {
					open_list.push_back(neighbor);
				}
			}
			neighbors.clear();

			// mark the current cell as visited
			// TODO: Record the cost of reaching this cell
			visited.insert(current);
		}

		// check if the visited cells are connected to another portal
		std::vector<std::shared_ptr<Portal>> connected_portals;
		for (auto &exit : this->portals) {
			if (exit->get_id() == portal->get_id()) {
				// skip the current portal
				continue;
			}

			// get the start cell of the exit portal
			// we only have to check one cell since the flood fill
			// should reach any exit cell
			auto exit_start = exit->get_entry_start(this->id);
			auto exit_cell = exit_start.ne + exit_start.se * this->cost_field->get_size();

			// check if the exit cell is connected to the visited cells
			if (visited.contains(exit_cell)) {
				connected_portals.push_back(exit);
			}
		}

		// set the exits for the current portal
		portal->set_exits(this->id, connected_portals);

		// All connected portals share the same exits
		// so we can connect them here
		for (auto &connected : connected_portals) {
			// make a new vector with all connected portals except the current one
			std::vector<std::shared_ptr<Portal>> other_connected;
			for (auto &other : connected_portals) {
				if (other->get_id() != connected->get_id()) {
					other_connected.push_back(other);
				}
			}

			// add the original portal as it is not in the connected portals vector
			other_connected.push_back(portal);

			// set the exits for the connected portal
			connected->set_exits(this->id, other_connected);

			// we don't need to food fill for this portal since we have
			// found all exits, so we can remove it from the portals that
			// should be searched
			portal_ids.erase(connected->get_id());
		}
	}
}

} // namespace openage::path
