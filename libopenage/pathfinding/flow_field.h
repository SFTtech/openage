// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <bitset>
#include <cstddef>
#include <memory>
#include <unordered_set>
#include <vector>

#include "error/error.h"
#include "log/log.h"

#include "coord/tile.h"

#include "pathfinding/definitions.h"
#include "pathfinding/integration_field.h"
#include "pathfinding/portal.h"
#include "pathfinding/types.h"


namespace openage {
namespace coord {
struct tile_delta;
} // namespace coord

namespace path {

template <size_t SECTOR_SIDE_LENGTH>
class IntegrationField;

class Portal;

template <size_t SECTOR_SIDE_LENGTH>
class FlowField {
public:
	/**
	 * Create a square flow field with a specified size.
	 */
	FlowField();

	/**
	 * Create a flow field from an existing integration field.
	 *
	 * @param integration_field Integration field.
	 */
	FlowField(const std::shared_ptr<IntegrationField<SECTOR_SIDE_LENGTH>> &integration_field);

	/**
	 * Get the size of the flow field.
	 *
	 * @return Size of the flow field.
	 */
	constexpr size_t get_size() const;

	/**
	 * Get the flow field value at a specified position.
	 *
	 * @param pos Coordinates of the cell (relative to field origin).
	 *
	 * @return Flowfield value at the specified position.
	 */
	flow_t get_cell(const coord::tile_delta &pos) const;

	/**
	 * Get the flow field value at a specified position.
	 *
	 * @param x X-coordinate of the cell.
	 * @param y Y-coordinate of the cell.
	 *
	 * @return Flowfield value at the specified position.
	 */
	flow_t get_cell(size_t x, size_t y) const;

	/**
	 * Get the flow field direction at a specified position.
	 *
	 * @param idx Index of the cell.
	 *
	 * @return Flowfield value at the specified position.
	 */
	flow_t get_cell(size_t idx) const;

	/**
	 * Get the flow field direction at a specified position.
	 *
	 * @param pos Coordinates of the cell (relative to field origin).
	 *
	 * @return Flowfield direction at the specified position.
	 */
	flow_dir_t get_dir(const coord::tile_delta &pos) const;

	/**
	 * Get the flow field direction at a specified position.
	 *
	 * @param x X-coordinate of the cell.
	 * @param y Y-coordinate of the cell.
	 *
	 * @return Flowfield direction at the specified position.
	 */
	flow_dir_t get_dir(size_t x, size_t y) const;

	/**
	 * Get the flow field direction at a specified position.
	 *
	 * @param idx Index of the cell.
	 *
	 * @return Flowfield direction at the specified position.
	 */
	flow_dir_t get_dir(size_t idx) const;

	/**
	 * Build the flow field.
	 *
	 * @param integration_field Integration field.
	 */
	void build(const std::shared_ptr<IntegrationField<SECTOR_SIDE_LENGTH>> &integration_field);

	/**
	 * Build the flow field for a portal.
	 *
	 * @param integration_field Integration field.
	 * @param other Integration field of the other sector.
	 * @param other_sector_id Sector ID of the other field.
	 * @param portal Portal connecting the two fields.
	 */
	void build(const std::shared_ptr<IntegrationField<SECTOR_SIDE_LENGTH>> &integration_field,
	           const std::shared_ptr<IntegrationField<SECTOR_SIDE_LENGTH>> &other,
	           sector_id_t other_sector_id,
	           const std::shared_ptr<Portal> &portal);

	/**
	 * Get the flow field values.
	 *
	 * @return Flow field values.
	 */
	const std::array<flow_t, SECTOR_SIDE_LENGTH * SECTOR_SIDE_LENGTH> &get_cells() const;

	/**
	 * Reset the flow field values for rebuilding the field.
	 */
	void reset();

	/**
	 * Reset all flags that are dependent on the path target location.
	 *
	 * These flags should be removed when the field is cached and reused for
	 * other targets.
	 *
	 * Relevant flags are:
	 * - FLOW_LOS_MASK
	 * - FLOW_WAVEFRONT_BLOCKED_MASK
	 */
	void reset_dynamic_flags();

	/**
	 * Transfer dynamic flags from an integration field.
	 *
	 * These flags should be transferred when the field is copied from cache.
	 * Flow field directions are not altered.
	 *
	 * Relevant flags are:
	 * - FLOW_LOS_MASK
	 * - FLOW_WAVEFRONT_BLOCKED_MASK
	 *
	 * @param integration_field Integration field.
	 */
	void transfer_dynamic_flags(const std::shared_ptr<IntegrationField<SECTOR_SIDE_LENGTH>> &integration_field);

private:
	/**
	 * Flow field cells.
	 */
	std::array<flow_t, SECTOR_SIDE_LENGTH * SECTOR_SIDE_LENGTH> cells;
};


template <size_t SECTOR_SIDE_LENGTH>
FlowField<SECTOR_SIDE_LENGTH>::FlowField() {
	cells.fill(FLOW_INIT);
	log::log(DBG << "Created flow field with size " << SECTOR_SIDE_LENGTH << "x" << SECTOR_SIDE_LENGTH);
}

template <size_t SECTOR_SIDE_LENGTH>
FlowField<SECTOR_SIDE_LENGTH>::FlowField(const std::shared_ptr<IntegrationField<SECTOR_SIDE_LENGTH>> &integration_field) {
	cells.fill(SECTOR_SIDE_LENGTH * SECTOR_SIDE_LENGTH, FLOW_INIT);
	this->build(integration_field);
}

template <size_t SECTOR_SIDE_LENGTH>
constexpr size_t FlowField<SECTOR_SIDE_LENGTH>::get_size() const {
	return SECTOR_SIDE_LENGTH;
}

template <size_t SECTOR_SIDE_LENGTH>
flow_t FlowField<SECTOR_SIDE_LENGTH>::get_cell(const coord::tile_delta &pos) const {
	return this->cells.at(pos.ne + pos.se * SECTOR_SIDE_LENGTH);
}

template <size_t SECTOR_SIDE_LENGTH>
flow_t FlowField<SECTOR_SIDE_LENGTH>::get_cell(size_t x, size_t y) const {
	return this->cells.at(x + y * SECTOR_SIDE_LENGTH);
}

template <size_t SECTOR_SIDE_LENGTH>
flow_t FlowField<SECTOR_SIDE_LENGTH>::get_cell(size_t idx) const {
	return this->cells.at(idx);
}

template <size_t SECTOR_SIDE_LENGTH>
flow_dir_t FlowField<SECTOR_SIDE_LENGTH>::get_dir(const coord::tile_delta &pos) const {
	return static_cast<flow_dir_t>(this->get_cell(pos) & FLOW_DIR_MASK);
}

template <size_t SECTOR_SIDE_LENGTH>
flow_dir_t FlowField<SECTOR_SIDE_LENGTH>::get_dir(size_t x, size_t y) const {
	return static_cast<flow_dir_t>(this->get_cell(x, y) & FLOW_DIR_MASK);
}

template <size_t SECTOR_SIDE_LENGTH>
flow_dir_t FlowField<SECTOR_SIDE_LENGTH>::get_dir(size_t idx) const {
	return static_cast<flow_dir_t>(this->get_cell(idx) & FLOW_DIR_MASK);
}

template <size_t SECTOR_SIDE_LENGTH>
void FlowField<SECTOR_SIDE_LENGTH>::build(const std::shared_ptr<IntegrationField<SECTOR_SIDE_LENGTH>> &integration_field) {
	ENSURE(integration_field->get_size() == this->get_size(),
	       "integration field size "
	           << integration_field->get_size() << "x" << integration_field->get_size()
	           << " does not match flow field size "
	           << this->get_size() << "x" << this->get_size());

	auto &integrate_cells = integration_field->get_cells();
	auto &flow_cells = this->cells;

	for (size_t y = 0; y < SECTOR_SIDE_LENGTH; ++y) {
		for (size_t x = 0; x < SECTOR_SIDE_LENGTH; ++x) {
			size_t idx = y * SECTOR_SIDE_LENGTH + x;

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
				auto cost = integrate_cells[idx - SECTOR_SIDE_LENGTH].cost;
				if (cost == INTEGRATED_COST_UNREACHABLE) {
					directions_unreachable |= 0x01;
				}
				else if (cost < smallest_cost) {
					smallest_cost = cost;
					direction = flow_dir_t::NORTH;
				}
			}
			if (x < SECTOR_SIDE_LENGTH - 1) {
				auto cost = integrate_cells[idx + 1].cost;
				if (cost == INTEGRATED_COST_UNREACHABLE) {
					directions_unreachable |= 0x02;
				}
				else if (cost < smallest_cost) {
					smallest_cost = cost;
					direction = flow_dir_t::EAST;
				}
			}
			if (y < SECTOR_SIDE_LENGTH - 1) {
				auto cost = integrate_cells[idx + SECTOR_SIDE_LENGTH].cost;
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
			if (x < SECTOR_SIDE_LENGTH - 1 and y > 0
			    and not(directions_unreachable & 0x01 and directions_unreachable & 0x02)) {
				auto cost = integrate_cells[idx - SECTOR_SIDE_LENGTH + 1].cost;
				if (cost < smallest_cost) {
					smallest_cost = cost;
					direction = flow_dir_t::NORTH_EAST;
				}
			}
			if (x < SECTOR_SIDE_LENGTH - 1 and y < SECTOR_SIDE_LENGTH - 1
			    and not(directions_unreachable & 0x02 and directions_unreachable & 0x04)) {
				auto cost = integrate_cells[idx + SECTOR_SIDE_LENGTH + 1].cost;
				if (cost < smallest_cost) {
					smallest_cost = cost;
					direction = flow_dir_t::SOUTH_EAST;
				}
			}
			if (x > 0 and y < SECTOR_SIDE_LENGTH - 1
			    and not(directions_unreachable & 0x04 and directions_unreachable & 0x08)) {
				auto cost = integrate_cells[idx + SECTOR_SIDE_LENGTH - 1].cost;
				if (cost < smallest_cost) {
					smallest_cost = cost;
					direction = flow_dir_t::SOUTH_WEST;
				}
			}
			if (x > 0 and y > 0
			    and not(directions_unreachable & 0x01 and directions_unreachable & 0x08)) {
				auto cost = integrate_cells[idx - SECTOR_SIDE_LENGTH - 1].cost;
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

template <size_t SECTOR_SIDE_LENGTH>
void FlowField<SECTOR_SIDE_LENGTH>::build(const std::shared_ptr<IntegrationField<SECTOR_SIDE_LENGTH>> &integration_field,
                                          const std::shared_ptr<IntegrationField<SECTOR_SIDE_LENGTH>> & /* other */,
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
				auto idx = y * SECTOR_SIDE_LENGTH + x;
				flow_cells[idx] = flow_cells[idx] | FLOW_PATHABLE_MASK;
				flow_cells[idx] = flow_cells[idx] | static_cast<uint8_t>(flow_dir_t::NORTH);
			}
		}
		else {
			auto y = exit_start.se;
			for (auto x = exit_start.ne; x <= exit_end.ne; ++x) {
				auto idx = y * SECTOR_SIDE_LENGTH + x;
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
				auto idx = y * SECTOR_SIDE_LENGTH + x;
				flow_cells[idx] = flow_cells[idx] | FLOW_PATHABLE_MASK;
				flow_cells[idx] = flow_cells[idx] | static_cast<uint8_t>(flow_dir_t::EAST);
			}
		}
		else {
			auto x = exit_start.ne;
			for (auto y = exit_start.se; y <= exit_end.se; ++y) {
				auto idx = y * SECTOR_SIDE_LENGTH + x;
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

template <size_t SECTOR_SIDE_LENGTH>
const std::array<flow_t, SECTOR_SIDE_LENGTH * SECTOR_SIDE_LENGTH> &FlowField<SECTOR_SIDE_LENGTH>::get_cells() const {
	return this->cells;
}

template <size_t SECTOR_SIDE_LENGTH>
void FlowField<SECTOR_SIDE_LENGTH>::reset() {
	std::fill(this->cells.begin(), this->cells.end(), FLOW_INIT);

	log::log(DBG << "Flow field has been reset");
}

template <size_t SECTOR_SIDE_LENGTH>
void FlowField<SECTOR_SIDE_LENGTH>::reset_dynamic_flags() {
	flow_t mask = 0xFF & ~(FLOW_LOS_MASK);
	for (flow_t &cell : this->cells) {
		cell = cell & mask;
	}

	log::log(DBG << "Flow field dynamic flags have been reset");
}

template <size_t SECTOR_SIDE_LENGTH>
void FlowField<SECTOR_SIDE_LENGTH>::transfer_dynamic_flags(const std::shared_ptr<IntegrationField<SECTOR_SIDE_LENGTH>> &integration_field) {
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

} // namespace path
} // namespace openage
