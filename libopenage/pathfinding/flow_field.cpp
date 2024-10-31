// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "flow_field.h"

#include <bitset>

#include "error/error.h"
#include "log/log.h"

#include "coord/tile.h"
#include "pathfinding/integration_field.h"
#include "pathfinding/portal.h"


namespace openage::path {

FlowField::FlowField(size_t size) :
	size{size},
	cells(this->size * this->size, FLOW_INIT) {
	log::log(DBG << "Created flow field with size " << this->size << "x" << this->size);
}

FlowField::FlowField(const std::shared_ptr<IntegrationField> &integration_field) :
	size{integration_field->get_size()},
	cells(this->size * this->size, FLOW_INIT) {
	this->build(integration_field);
}

size_t FlowField::get_size() const {
	return this->size;
}

flow_t FlowField::get_cell(const coord::tile_delta &pos) const {
	return this->cells.at(pos.ne + pos.se * this->size);
}

flow_t FlowField::get_cell(size_t x, size_t y) const {
	return this->cells.at(x + y * this->size);
}

flow_t FlowField::get_cell(size_t idx) const {
	return this->cells.at(idx);
}

flow_dir_t FlowField::get_dir(const coord::tile_delta &pos) const {
	return static_cast<flow_dir_t>(this->get_cell(pos) & FLOW_DIR_MASK);
}

flow_dir_t FlowField::get_dir(size_t x, size_t y) const {
	return static_cast<flow_dir_t>(this->get_cell(x, y) & FLOW_DIR_MASK);
}

flow_dir_t FlowField::get_dir(size_t idx) const {
	return static_cast<flow_dir_t>(this->get_cell(idx) & FLOW_DIR_MASK);
}

void FlowField::build(const std::shared_ptr<IntegrationField> &integration_field) {
	ENSURE(integration_field->get_size() == this->get_size(),
	       "integration field size "
	           << integration_field->get_size() << "x" << integration_field->get_size()
	           << " does not match flow field size "
	           << this->get_size() << "x" << this->get_size());

	auto &integrate_cells = integration_field->get_cells();
	auto &flow_cells = this->cells;

	for (size_t y = 0; y < this->size; ++y) {
		for (size_t x = 0; x < this->size; ++x) {
			size_t idx = y * this->size + x;

			const auto &integrate_cell = integrate_cells[idx];
			auto &flow_cell = flow_cells[idx];

			if (integrate_cell.cost == INTEGRATED_COST_UNREACHABLE) {
				// Cell cannot be used as path
				continue;
			}

			flow_t transfer_flags = integrate_cell.flags & FLOW_FLAGS_MASK;
			flow_cell |= transfer_flags;

			if (flow_cell & FLOW_TARGET_MASK) {
				// target cells are pathable
				flow_cell |= FLOW_PATHABLE_MASK;

				// they also have a preset flow direction so we can skip here
				continue;
			}

			// Store which of the non-diagonal directions are unreachable.
			// north == 0x01, east == 0x02, south == 0x04, west == 0x08
			uint8_t directions_unreachable = 0x00;

			// Find the neighbor with the smallest cost.
			flow_dir_t direction = static_cast<flow_dir_t>(flow_cell & FLOW_DIR_MASK);
			auto smallest_cost = INTEGRATED_COST_UNREACHABLE;

			// Cardinal directions
			if (y > 0) {
				auto cost = integrate_cells[idx - this->size].cost;
				if (cost == INTEGRATED_COST_UNREACHABLE) {
					directions_unreachable |= 0x01;
				}
				else if (cost < smallest_cost) {
					smallest_cost = cost;
					direction = flow_dir_t::NORTH;
				}
			}
			if (x < this->size - 1) {
				auto cost = integrate_cells[idx + 1].cost;
				if (cost == INTEGRATED_COST_UNREACHABLE) {
					directions_unreachable |= 0x02;
				}
				else if (cost < smallest_cost) {
					smallest_cost = cost;
					direction = flow_dir_t::EAST;
				}
			}
			if (y < this->size - 1) {
				auto cost = integrate_cells[idx + this->size].cost;
				if (cost == INTEGRATED_COST_UNREACHABLE) {
					directions_unreachable |= 0x04;
				}
				else if (cost < smallest_cost) {
					smallest_cost = cost;
					direction = flow_dir_t::SOUTH;
				}
			}
			if (x > 0) {
				auto cost = integrate_cells[idx - 1].cost;
				if (cost == INTEGRATED_COST_UNREACHABLE) {
					directions_unreachable |= 0x08;
				}
				else if (cost < smallest_cost) {
					smallest_cost = cost;
					direction = flow_dir_t::WEST;
				}
			}

			// Diagonal directions
			if (x < this->size - 1 and y > 0
			    and not(directions_unreachable & 0x01 and directions_unreachable & 0x02)) {
				auto cost = integrate_cells[idx - this->size + 1].cost;
				if (cost < smallest_cost) {
					smallest_cost = cost;
					direction = flow_dir_t::NORTH_EAST;
				}
			}
			if (x < this->size - 1 and y < this->size - 1
			    and not(directions_unreachable & 0x02 and directions_unreachable & 0x04)) {
				auto cost = integrate_cells[idx + this->size + 1].cost;
				if (cost < smallest_cost) {
					smallest_cost = cost;
					direction = flow_dir_t::SOUTH_EAST;
				}
			}
			if (x > 0 and y < this->size - 1
			    and not(directions_unreachable & 0x04 and directions_unreachable & 0x08)) {
				auto cost = integrate_cells[idx + this->size - 1].cost;
				if (cost < smallest_cost) {
					smallest_cost = cost;
					direction = flow_dir_t::SOUTH_WEST;
				}
			}
			if (x > 0 and y > 0
			    and not(directions_unreachable & 0x01 and directions_unreachable & 0x08)) {
				auto cost = integrate_cells[idx - this->size - 1].cost;
				if (cost < smallest_cost) {
					smallest_cost = cost;
					direction = flow_dir_t::NORTH_WEST;
				}
			}

			// Set the flow field cell to pathable.
			flow_cell |= FLOW_PATHABLE_MASK;

			// Set the flow field cell to the direction of the smallest cost.
			flow_cell |= static_cast<uint8_t>(direction);
		}
	}
}

void FlowField::build(const std::shared_ptr<IntegrationField> &integration_field,
                      const std::shared_ptr<IntegrationField> & /* other */,
                      sector_id_t other_sector_id,
                      const std::shared_ptr<Portal> &portal) {
	ENSURE(integration_field->get_size() == this->get_size(),
	       "integration field size "
	           << integration_field->get_size() << "x" << integration_field->get_size()
	           << " does not match flow field size "
	           << this->get_size() << "x" << this->get_size());

	auto &flow_cells = this->cells;
	auto direction = portal->get_direction();

	// portal entry and exit cell coordinates
	auto entry_start = portal->get_entry_start(other_sector_id);
	auto exit_start = portal->get_exit_start(other_sector_id);
	auto exit_end = portal->get_exit_end(other_sector_id);

	// TODO: Compare integration values from other side of portal
	// auto &integrate_cells = integration_field->get_cells();

	// set the direction for the flow field cells that are part of the portal
	if (direction == PortalDirection::NORTH_SOUTH) {
		bool other_is_north = entry_start.se > exit_start.se;
		if (other_is_north) {
			auto y = exit_start.se;
			for (auto x = exit_start.ne; x <= exit_end.ne; ++x) {
				auto idx = y * this->size + x;
				flow_cells[idx] = flow_cells[idx] | FLOW_PATHABLE_MASK;
				flow_cells[idx] = flow_cells[idx] | static_cast<uint8_t>(flow_dir_t::NORTH);
			}
		}
		else {
			auto y = exit_start.se;
			for (auto x = exit_start.ne; x <= exit_end.ne; ++x) {
				auto idx = y * this->size + x;
				flow_cells[idx] = flow_cells[idx] | FLOW_PATHABLE_MASK;
				flow_cells[idx] = flow_cells[idx] | static_cast<uint8_t>(flow_dir_t::SOUTH);
			}
		}
	}
	else if (direction == PortalDirection::EAST_WEST) {
		bool other_is_east = entry_start.ne < exit_start.ne;
		if (other_is_east) {
			auto x = exit_start.ne;
			for (auto y = exit_start.se; y <= exit_end.se; ++y) {
				auto idx = y * this->size + x;
				flow_cells[idx] = flow_cells[idx] | FLOW_PATHABLE_MASK;
				flow_cells[idx] = flow_cells[idx] | static_cast<uint8_t>(flow_dir_t::EAST);
			}
		}
		else {
			auto x = exit_start.ne;
			for (auto y = exit_start.se; y <= exit_end.se; ++y) {
				auto idx = y * this->size + x;
				flow_cells[idx] = flow_cells[idx] | FLOW_PATHABLE_MASK;
				flow_cells[idx] = flow_cells[idx] | static_cast<uint8_t>(flow_dir_t::WEST);
			}
		}
	}
	else {
		throw Error(ERR << "Invalid portal direction: " << static_cast<int>(direction));
	}

	this->build(integration_field);
}

const std::vector<flow_t> &FlowField::get_cells() const {
	return this->cells;
}

void FlowField::reset() {
	std::fill(this->cells.begin(), this->cells.end(), FLOW_INIT);

	log::log(DBG << "Flow field has been reset");
}

void FlowField::reset_dynamic_flags() {
	flow_t mask = 0xFF & ~(FLOW_LOS_MASK);
	for (flow_t &cell : this->cells) {
		cell = cell & mask;
	}

	log::log(DBG << "Flow field dynamic flags have been reset");
}

void FlowField::transfer_dynamic_flags(const std::shared_ptr<IntegrationField> &integration_field) {
	auto &integrate_cells = integration_field->get_cells();
	auto &flow_cells = this->cells;

	for (size_t idx = 0; idx < integrate_cells.size(); ++idx) {
		if (integrate_cells[idx].flags & INTEGRATE_LOS_MASK) {
			// Cell is in line of sight
			flow_cells[idx] |= FLOW_LOS_MASK;
		}
	}

	log::log(DBG << "Flow field dynamic flags have been transferred");
}

} // namespace openage::path
