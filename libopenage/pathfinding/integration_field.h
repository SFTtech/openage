// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <array>
#include <cmath>
#include <cstddef>
#include <deque>
#include <memory>
#include <unordered_set>
#include <vector>

#include "coord/tile.h"

#include "error/error.h"

#include "log/log.h"

#include "pathfinding/cost_field.h"
#include "pathfinding/definitions.h"
#include "pathfinding/portal.h"
#include "pathfinding/types.h"

namespace openage {
namespace coord {
struct tile_delta;
} // namespace coord

namespace path {

template <size_t SECTOR_SIDE_LENGTH>
class CostField;

class Portal;

/**
 * Integration field in the flow-field pathfinding algorithm.
 */
template <size_t SECTOR_SIDE_LENGTH>
class IntegrationField {
public:
	/**
	 * Create a square integration field.
	 */
	IntegrationField();

	/**
	 * Get the size of the integration field.
	 *
	 * @return Size of the integration field.
	 */
	size_t get_size() const;

	/**
	 * Get the integration value at a specified position.
	 *
	 * @param pos Coordinates of the cell (relative to field origin).
	 * @return Integration value at the specified position.
	 */
	const integrated_t &get_cell(const coord::tile_delta &pos) const;

	/**
	 * Get the integration value at a specified position.
	 *
	 * @param x X-coordinate of the cell.
	 * @param y Y-coordinate of the cell.
	 * @return Integration value at the specified position.
	 */
	const integrated_t &get_cell(size_t x, size_t y) const;

	/**
	 * Get the integration value at a specified position.
	 *
	 * @param idx Index of the cell.
	 * @return Integration value at the specified position.
	 */
	const integrated_t &get_cell(size_t idx) const;

	/**
	 * Calculate the line-of-sight integration flags for a target cell.
	 *
	 * The target cell coordinates must lie within the field.
	 *
	 * Returns a list of cells that are flagged as "wavefront blocked". These cells
	 * can be used as a starting point for the cost integration.
	 *
	 * @param cost_field Cost field to integrate.
	 * @param target Coordinates of the target cell (relative to field origin).
	 *
	 * @return Cells flagged as "wavefront blocked".
	 */
	std::vector<size_t> integrate_los(const std::shared_ptr<CostField<SECTOR_SIDE_LENGTH>> &cost_field,
	                                  const coord::tile_delta &target);

	/**
	 * Calculate the line-of-sight integration flags starting from a portal to another
	 * integration field.
	 *
	 * Returns a list of cells that are flagged as "wavefront blocked". These cells
	 * can be used as a starting point for the cost integration.
	 *
	 * @param cost_field Cost field to integrate.
	 * @param other Integration field of the other sector.
	 * @param other_sector_id Sector ID of the other integration field.
	 * @param portal Portal connecting the two fields.
	 * @param target Coordinates of the target cell (relative to field origin).
	 *
	 * @return Cells flagged as "wavefront blocked".
	 */
	std::vector<size_t> integrate_los(const std::shared_ptr<CostField<SECTOR_SIDE_LENGTH>> &cost_field,
	                                  const std::shared_ptr<IntegrationField<SECTOR_SIDE_LENGTH>> &other,
	                                  sector_id_t other_sector_id,
	                                  const std::shared_ptr<Portal> &portal,
	                                  const coord::tile_delta &target);

	/**
	 * Calculate the line-of-sight integration flags for a target cell.
	 *
	 * Returns a list of cells that are flagged as "wavefront blocked". These cells
	 * can be used as a starting point for the cost integration.
	 *
	 * @param cost_field Cost field to integrate.
	 * @param target Coordinates of the target cell (relative to field origin).
	 * @param start_cost Integration cost for the start wave.
	 * @param start_wave Cells used for the first LOS integration wave. The wavefront
	 * 					 expands outwards from these cells.
	 *
	 * @return Cells flagged as "wavefront blocked".
	 */
	std::vector<size_t> integrate_los(const std::shared_ptr<CostField<SECTOR_SIDE_LENGTH>> &cost_field,
	                                  const coord::tile_delta &target,
	                                  integrated_cost_t start_cost,
	                                  std::vector<size_t> &&start_wave);

	/**
	 * Calculate the cost integration field starting from a target cell.
	 *
	 * @param cost_field Cost field to integrate.
	 * @param target Coordinates of the target cell.
	 */
	void integrate_cost(const std::shared_ptr<CostField<SECTOR_SIDE_LENGTH>> &cost_field,
	                    const coord::tile_delta &target);

	/**
	 * Calculate the cost integration field starting from a portal to another
	 * integration field.
	 *
	 * @param cost_field Cost field to integrate.
	 * @param other_sector_id Sector ID of the other integration field.
	 * @param portal Portal connecting the two fields.
	 */
	void integrate_cost(const std::shared_ptr<CostField<SECTOR_SIDE_LENGTH>> &cost_field,
	                    sector_id_t other_sector_id,
	                    const std::shared_ptr<Portal> &portal);

	/**
	 * Calculate the cost integration field starting from a wavefront.
	 *
	 * @param cost_field Cost field to integrate.
	 * @param start_cells Cells flagged as "wavefront blocked" from a LOS pass.
	 */
	void integrate_cost(const std::shared_ptr<CostField<SECTOR_SIDE_LENGTH>> &cost_field,
	                    std::vector<size_t> &&start_cells);

	/**
	 * Get the integration field values.
	 *
	 * @return Integration field values.
	 */
	const std::array<integrated_t, SECTOR_SIDE_LENGTH * SECTOR_SIDE_LENGTH> &get_cells() const;

	/**
	 * Reset the integration field for a new integration.
	 */
	void reset();

	/**
	 * Reset all flags that are dependent on the path target location. These
	 * flags should be removed when the field is cached and reused for
	 * other targets.
	 *
	 * Relevant flags are:
	 * - INTEGRATE_LOS_MASK
	 * - INTEGRATE_WAVEFRONT_BLOCKED_MASK
	 * - INTEGRATE_FOUND_MASK
	 */
	void reset_dynamic_flags();

private:
	/**
	 * Update a neigbor cell during the cost integration process.
	 *
	 * @param idx Index of the neighbor cell that is updated.
	 * @param cell_cost Cost of the neighbor cell from the cost field.
	 * @param integrated_cost Current integrated cost of the updating cell in the integration field.
	 * @param wave List of cells that are part of the next wavefront.
	 *
	 * @return New integration value of the cell.
	 */
	void update_neighbor(size_t idx,
	                     cost_t cell_cost,
	                     integrated_cost_t integrated_cost,
	                     std::vector<size_t> &wave);

	/**
	 * Get the LOS corners around a cell.
	 *
	 * @param cost_field Cost field to integrate.
	 * @param target Cell coordinates of the target (relative to field origin).
	 * @param blocker Cell coordinates of the cell blocking LOS (relative to field origin).
	 *
	 * @return Field coordinates of the LOS corners.
	 */
	std::vector<std::pair<int, int>> get_los_corners(const std::shared_ptr<CostField<SECTOR_SIDE_LENGTH>> &cost_field,
	                                                 const coord::tile_delta &target,
	                                                 const coord::tile_delta &blocker);

	/**
	 * Get the cells in a bresenham's line between the corner cell and the field edge.
	 *
	 * This function is a modified version of the bresenham's line algorithm that
	 * retrieves the cells between the corner point and the field's edge, rather than
	 * the cells between two arbitrary points. We do this because the intersection
	 * point with the field edge is unknown.
	 *
	 * @param target Cell coordinates of the target (relative to field origin).
	 * @param corner_x X field coordinate edge of the LOS corner.
	 * @param corner_y Y field coordinate edge of the LOS corner.
	 *
	 * @return Cell indices of the LOS line.
	 */
	std::vector<size_t> bresenhams_line(const coord::tile_delta &target,
	                                    int corner_x,
	                                    int corner_y);

	/**
	 * Integration field values.
	 */
	std::array<integrated_t, SECTOR_SIDE_LENGTH * SECTOR_SIDE_LENGTH> cells;
};

template <size_t SECTOR_SIDE_LENGTH>
IntegrationField<SECTOR_SIDE_LENGTH>::IntegrationField() {
	cells.fill(INTEGRATE_INIT);
	log::log(DBG << "Created integration field with size " << SECTOR_SIDE_LENGTH << "x" << SECTOR_SIDE_LENGTH);
}

template <size_t SECTOR_SIDE_LENGTH>
size_t IntegrationField<SECTOR_SIDE_LENGTH>::get_size() const {
	return SECTOR_SIDE_LENGTH;
}

template <size_t SECTOR_SIDE_LENGTH>
const integrated_t &IntegrationField<SECTOR_SIDE_LENGTH>::get_cell(const coord::tile_delta &pos) const {
	return this->cells.at(pos.ne + pos.se * SECTOR_SIDE_LENGTH);
}

template <size_t SECTOR_SIDE_LENGTH>
const integrated_t &IntegrationField<SECTOR_SIDE_LENGTH>::get_cell(size_t x, size_t y) const {
	return this->cells.at(x + y * SECTOR_SIDE_LENGTH);
}

template <size_t SECTOR_SIDE_LENGTH>
const integrated_t &IntegrationField<SECTOR_SIDE_LENGTH>::get_cell(size_t idx) const {
	return this->cells.at(idx);
}

template <size_t SECTOR_SIDE_LENGTH>
std::vector<size_t> IntegrationField<SECTOR_SIDE_LENGTH>::integrate_los(const std::shared_ptr<CostField<SECTOR_SIDE_LENGTH>> &cost_field,
                                                       const coord::tile_delta &target) {
	ENSURE(cost_field->get_size() == this->get_size(),
	       "cost field size "
	           << cost_field->get_size() << "x" << cost_field->get_size()
	           << " does not match integration field size "
	           << this->get_size() << "x" << this->get_size());

	ENSURE(target.ne >= 0
	           and target.se >= 0
	           and target.ne < static_cast<coord::tile_t>(SECTOR_SIDE_LENGTH)
	           and target.se < static_cast<coord::tile_t>(SECTOR_SIDE_LENGTH),
	       "target cell (" << target.ne << ", " << target.se << ") "
	                       << "is out of bounds for integration field of size "
	                       << SECTOR_SIDE_LENGTH << "x" << SECTOR_SIDE_LENGTH);

	std::vector<size_t> start_cells;
	integrated_cost_t start_cost = INTEGRATED_COST_START;

	// Target cell index
	auto target_idx = target.ne + target.se * SECTOR_SIDE_LENGTH;

	this->cells[target_idx].cost = start_cost;
	this->cells[target_idx].flags |= INTEGRATE_TARGET_MASK;

	if (cost_field->get_cost(target_idx) > COST_MIN) {
		// Do a preliminary LOS integration wave for targets that have cost > min cost
		// This avoids the bresenham's line algorithm calculations
		// (which wouldn't return accurate results for blocker == target)
		// and makes sure that sorrounding cells that are min cost are considered
		// in line-of-sight.

		this->cells[target_idx].flags |= INTEGRATE_FOUND_MASK;
		this->cells[target_idx].flags |= INTEGRATE_LOS_MASK;

		// Add neighbors to current wave
		if (target.se > 0) {
			start_cells.push_back(target_idx - SECTOR_SIDE_LENGTH);
		}
		if (target.ne > 0) {
			start_cells.push_back(target_idx - 1);
		}
		if (target.se < static_cast<coord::tile_t>(SECTOR_SIDE_LENGTH - 1)) {
			start_cells.push_back(target_idx + SECTOR_SIDE_LENGTH);
		}
		if (target.ne < static_cast<coord::tile_t>(SECTOR_SIDE_LENGTH - 1)) {
			start_cells.push_back(target_idx + 1);
		}

		// Increment wave cost as we technically handled the first wave in this block
		start_cost += 1;
	}
	else {
		// Move outwards from the target cell, updating the integration field
		start_cells.push_back(target_idx);
	}

	return this->integrate_los(cost_field, target, start_cost, std::move(start_cells));
}

template <size_t SECTOR_SIDE_LENGTH>
std::vector<size_t> IntegrationField<SECTOR_SIDE_LENGTH>::integrate_los(const std::shared_ptr<CostField<SECTOR_SIDE_LENGTH>> &cost_field,
                                                       const std::shared_ptr<IntegrationField<SECTOR_SIDE_LENGTH>> &other,
                                                       sector_id_t other_sector_id,
                                                       const std::shared_ptr<Portal> &portal,
                                                       const coord::tile_delta &target) {
	ENSURE(cost_field->get_size() == this->get_size(),
	       "cost field size "
	           << cost_field->get_size() << "x" << cost_field->get_size()
	           << " does not match integration field size "
	           << this->get_size() << "x" << this->get_size());

	std::vector<size_t> wavefront_blocked_portal;

	std::vector<size_t> start_cells;

	auto exit_start = portal->get_exit_start(other_sector_id);
	auto exit_end = portal->get_exit_end(other_sector_id);
	auto entry_start = portal->get_entry_start(other_sector_id);

	auto x_diff = exit_start.ne - entry_start.ne;
	auto y_diff = exit_start.se - entry_start.se;

	auto &cost_cells = cost_field->get_costs();
	auto &other_cells = other->get_cells();

	// transfer masks for flags from the other side of the portal
	// only LOS and wavefront blocked flags are relevant
	integrated_flags_t transfer_mask = INTEGRATE_LOS_MASK | INTEGRATE_WAVEFRONT_BLOCKED_MASK;

	// every portal cell is a target cell
	for (auto y = exit_start.se; y <= exit_end.se; ++y) {
		for (auto x = exit_start.ne; x <= exit_end.ne; ++x) {
			// cell index on the exit side of the portal
			auto target_idx = x + y * SECTOR_SIDE_LENGTH;

			// cell index on the entry side of the portal
			auto entry_idx = x - x_diff + (y - y_diff) * SECTOR_SIDE_LENGTH;

			// Set the cost of all target cells to the start value
			this->cells[target_idx].cost = INTEGRATED_COST_START;
			this->cells[target_idx].flags = other_cells[entry_idx].flags & transfer_mask;

			this->cells[target_idx].flags |= INTEGRATE_TARGET_MASK;

			if (not(this->cells[target_idx].flags & transfer_mask)) {
				// If neither LOS nor wavefront blocked flags are set for the portal entry,
				// the portal exit cell doesn't affect the LOS and we can skip further checks
				continue;
			}

			// Get the cost of the current cell
			auto cell_cost = cost_cells[target_idx];

			if (cell_cost > COST_MIN or this->cells[target_idx].flags & INTEGRATE_WAVEFRONT_BLOCKED_MASK) {
				// cell blocks line of sight

				// set the blocked flag for the cell if it wasn't set already
				this->cells[target_idx].flags |= INTEGRATE_WAVEFRONT_BLOCKED_MASK;
				wavefront_blocked_portal.push_back(target_idx);

				// set the found flag for the cell, so that the start costs
				// are not changed in the main LOS integration
				this->cells[target_idx].flags |= INTEGRATE_FOUND_MASK;

				// check each neighbor for a corner
				auto corners = this->get_los_corners(cost_field, target, coord::tile_delta(x, y));
				for (auto &corner : corners) {
					// draw a line from the corner to the edge of the field
					// to get the cells blocked by the corner
					auto blocked_cells = this->bresenhams_line(target, corner.first, corner.second);
					for (auto blocked_idx : blocked_cells) {
						if (cost_cells[blocked_idx] > COST_MIN) {
							// stop if blocked_idx is not min cost
							// because this idx may create a new corner
							break;
						}
						// set the blocked flag for the cell
						this->cells[blocked_idx].flags |= INTEGRATE_WAVEFRONT_BLOCKED_MASK;

						// clear los flag if it was set
						this->cells[blocked_idx].flags &= ~INTEGRATE_LOS_MASK;

						wavefront_blocked_portal.push_back(blocked_idx);
					}
				}
				continue;
			}

			// the cell has the LOS flag and is added to the start cells
			start_cells.push_back(target_idx);
		}
	}

	if (start_cells.empty()) {
		// Main LOS integration will not enter its loop
		// so we can take a shortcut and just return the
		// wavefront blocked cells we already found
		return wavefront_blocked_portal;
	}

	// Call main LOS integration function
	auto wavefront_blocked_main = this->integrate_los(cost_field,
	                                                  target,
	                                                  INTEGRATED_COST_START,
	                                                  std::move(start_cells));
	wavefront_blocked_main.insert(wavefront_blocked_main.end(),
	                              wavefront_blocked_portal.begin(),
	                              wavefront_blocked_portal.end());
	return wavefront_blocked_main;
}

template <size_t SECTOR_SIDE_LENGTH>
std::vector<size_t> IntegrationField<SECTOR_SIDE_LENGTH>::integrate_los(const std::shared_ptr<CostField<SECTOR_SIDE_LENGTH>> &cost_field,
                                                       const coord::tile_delta &target,
                                                       integrated_cost_t start_cost,
                                                       std::vector<size_t> &&start_wave) {
	ENSURE(cost_field->get_size() == this->get_size(),
	       "cost field size "
	           << cost_field->get_size() << "x" << cost_field->get_size()
	           << " does not match integration field size "
	           << this->get_size() << "x" << this->get_size());

	// Store the wavefront_blocked cells
	std::vector<size_t> wavefront_blocked;

	// Cells that still have to be visited by the current wave
	std::vector<size_t> current_wave = std::move(start_wave);

	// Cells that have to be visited in the next wave
	std::vector<size_t> next_wave;

	// Preallocate ~30% of the field size for the wavefront
	// This reduces the number of reallocations on push_back operations
	// TODO: Find "optimal" value for reserve
	current_wave.reserve(SECTOR_SIDE_LENGTH * 3);
	next_wave.reserve(SECTOR_SIDE_LENGTH * 3);

	// Cost of the current wave
	integrated_cost_t wave_cost = start_cost;

	// Get the cost field values
	auto &cost_cells = cost_field->get_costs();
	auto &integrate_cells = this->cells;

	do {
		for (size_t i = 0; i < current_wave.size(); ++i) {
			// inner loop: handle a wave
			auto idx = current_wave[i];
			auto &cell = integrate_cells[idx];

			if (cell.flags & INTEGRATE_FOUND_MASK) {
				// Skip cells that are already in the line of sight
				continue;
			}
			else if (cell.flags & INTEGRATE_WAVEFRONT_BLOCKED_MASK) {
				// Stop at cells that are blocked by a LOS corner
				cell.cost = wave_cost - 1 + cost_cells[idx];
				cell.flags |= INTEGRATE_FOUND_MASK;
				continue;
			}

			// Add the current cell to the found cells
			cell.flags |= INTEGRATE_FOUND_MASK;

			// Get the x and y coordinates of the current cell
			auto x = idx % SECTOR_SIDE_LENGTH;
			auto y = idx / SECTOR_SIDE_LENGTH;

			// Get the cost of the current cell
			auto cell_cost = cost_cells[idx];

			if (cell_cost > COST_MIN) {
				// cell blocks line of sight
				// and we have to check for corners
				if (cell_cost != COST_IMPASSABLE) {
					// Add the current cell to the blocked wavefront if it's not a wall
					wavefront_blocked.push_back(idx);
					cell.cost = wave_cost - 1 + cell_cost;
					cell.flags |= INTEGRATE_WAVEFRONT_BLOCKED_MASK;
				}

				// check each neighbor for a corner
				auto corners = this->get_los_corners(cost_field, target, coord::tile_delta(x, y));
				for (auto &corner : corners) {
					// draw a line from the corner to the edge of the field
					// to get the cells blocked by the corner
					auto blocked_cells = this->bresenhams_line(target, corner.first, corner.second);
					for (auto blocked_idx : blocked_cells) {
						if (cost_cells[blocked_idx] > COST_MIN) {
							// stop if blocked_idx is impassable
							break;
						}
						// set the blocked flag for the cell
						integrate_cells[blocked_idx].flags |= INTEGRATE_WAVEFRONT_BLOCKED_MASK;

						// clear los flag if it was set
						integrate_cells[blocked_idx].flags &= ~INTEGRATE_LOS_MASK;

						wavefront_blocked.push_back(blocked_idx);
					}
				}
				continue;
			}

			// The cell is in the line of sight at min cost
			// Set the LOS flag and cost
			cell.cost = wave_cost;
			cell.flags |= INTEGRATE_LOS_MASK;

			// Search the neighbors of the current cell
			if (y > 0) {
				auto neighbor_idx = idx - SECTOR_SIDE_LENGTH;
				next_wave.push_back(neighbor_idx);
			}
			if (x > 0) {
				auto neighbor_idx = idx - 1;
				next_wave.push_back(neighbor_idx);
			}
			if (y < SECTOR_SIDE_LENGTH - 1) {
				auto neighbor_idx = idx + SECTOR_SIDE_LENGTH;
				next_wave.push_back(neighbor_idx);
			}
			if (x < SECTOR_SIDE_LENGTH - 1) {
				auto neighbor_idx = idx + 1;
				next_wave.push_back(neighbor_idx);
			}
		}

		// increment the cost and advance the wavefront outwards
		wave_cost += 1;
		current_wave.swap(next_wave);
		next_wave.clear();
	}
	while (not current_wave.empty());

	return wavefront_blocked;
}
template <size_t SECTOR_SIDE_LENGTH>
void IntegrationField<SECTOR_SIDE_LENGTH>::integrate_cost(const std::shared_ptr<CostField<SECTOR_SIDE_LENGTH>> &cost_field,
                                         const coord::tile_delta &target) {
	ENSURE(cost_field->get_size() == this->get_size(),
	       "cost field size "
	           << cost_field->get_size() << "x" << cost_field->get_size()
	           << " does not match integration field size "
	           << this->get_size() << "x" << this->get_size());

	// Target cell index
	auto target_idx = target.ne + target.se * SECTOR_SIDE_LENGTH;

	// Move outwards from the target cell, updating the integration field
	this->cells[target_idx].cost = INTEGRATED_COST_START;
	this->cells[target_idx].flags |= INTEGRATE_TARGET_MASK;
	this->integrate_cost(cost_field, {target_idx});
}

template <size_t SECTOR_SIDE_LENGTH>
void IntegrationField<SECTOR_SIDE_LENGTH>::integrate_cost(const std::shared_ptr<CostField<SECTOR_SIDE_LENGTH>> &cost_field,
                                         sector_id_t other_sector_id,
                                         const std::shared_ptr<Portal> &portal) {
	ENSURE(cost_field->get_size() == this->get_size(),
	       "cost field size "
	           << cost_field->get_size() << "x" << cost_field->get_size()
	           << " does not match integration field size "
	           << this->get_size() << "x" << this->get_size());

	// Integrate the cost of the cells on the exit side (this) of the portal
	std::vector<size_t> start_cells;
	auto exit_start = portal->get_exit_start(other_sector_id);
	auto exit_end = portal->get_exit_end(other_sector_id);
	for (auto y = exit_start.se; y <= exit_end.se; ++y) {
		for (auto x = exit_start.ne; x <= exit_end.ne; ++x) {
			// every portal cell is a target cell
			auto target_idx = x + y * SECTOR_SIDE_LENGTH;

			// Set the cost of all target cells to the start value
			this->cells[target_idx].cost = INTEGRATED_COST_START;
			this->cells[target_idx].flags |= INTEGRATE_TARGET_MASK;
			start_cells.push_back(target_idx);

			// TODO: Transfer flags and cost from the other integration field
		}
	}

	// Integrate the rest of the cost field
	this->integrate_cost(cost_field, std::move(start_cells));
}

template <size_t SECTOR_SIDE_LENGTH>
void IntegrationField<SECTOR_SIDE_LENGTH>::integrate_cost(const std::shared_ptr<CostField<SECTOR_SIDE_LENGTH>> &cost_field,
                                         std::vector<size_t> &&start_cells) {
	// Cells that still have to be visited by the current wave
	std::vector<size_t> current_wave = std::move(start_cells);

	// Cells that have to be visited in the next wave
	std::vector<size_t> next_wave;

	// Preallocate ~30% of the field size for the wavefront
	// This reduces the number of reallocations on push_back operations
	// TODO: Find "optimal" value for reserve
	current_wave.reserve(SECTOR_SIDE_LENGTH * 3);
	next_wave.reserve(SECTOR_SIDE_LENGTH * 3);

	// Get the cost field values
	auto &cost_cells = cost_field->get_costs();

	// Move outwards from the wavefront, updating the integration field
	while (not current_wave.empty()) {
		for (size_t i = 0; i < current_wave.size(); ++i) {
			auto idx = current_wave[i];

			// Get the x and y coordinates of the current cell
			auto x = idx % SECTOR_SIDE_LENGTH;
			auto y = idx / SECTOR_SIDE_LENGTH;

			auto integrated_current = this->cells[idx].cost;

			// Get the neighbors of the current cell
			if (y > 0) {
				auto neighbor_idx = idx - SECTOR_SIDE_LENGTH;
				this->update_neighbor(neighbor_idx,
				                      cost_cells[neighbor_idx],
				                      integrated_current,
				                      next_wave);
			}
			if (x > 0) {
				auto neighbor_idx = idx - 1;
				this->update_neighbor(neighbor_idx,
				                      cost_cells[neighbor_idx],
				                      integrated_current,
				                      next_wave);
			}
			if (y < SECTOR_SIDE_LENGTH - 1) {
				auto neighbor_idx = idx + SECTOR_SIDE_LENGTH;
				this->update_neighbor(neighbor_idx,
				                      cost_cells[neighbor_idx],
				                      integrated_current,
				                      next_wave);
			}
			if (x < SECTOR_SIDE_LENGTH - 1) {
				auto neighbor_idx = idx + 1;
				this->update_neighbor(neighbor_idx,
				                      cost_cells[neighbor_idx],
				                      integrated_current,
				                      next_wave);
			}
		}

		current_wave.swap(next_wave);
		next_wave.clear();
	}
}
template <size_t SECTOR_SIDE_LENGTH>
const std::array<integrated_t, SECTOR_SIDE_LENGTH * SECTOR_SIDE_LENGTH> &IntegrationField<SECTOR_SIDE_LENGTH>::get_cells() const {
	return this->cells;
}

template <size_t SECTOR_SIDE_LENGTH>
void IntegrationField<SECTOR_SIDE_LENGTH>::reset() {
	std::fill(this->cells.begin(), this->cells.end(), INTEGRATE_INIT);

	log::log(DBG << "Integration field has been reset");
}

template <size_t SECTOR_SIDE_LENGTH>
void IntegrationField<SECTOR_SIDE_LENGTH>::reset_dynamic_flags() {
	integrated_flags_t mask = 0xFF & ~(INTEGRATE_LOS_MASK | INTEGRATE_WAVEFRONT_BLOCKED_MASK | INTEGRATE_FOUND_MASK);
	for (integrated_t &cell : this->cells) {
		cell.flags = cell.flags & mask;
	}

	log::log(DBG << "Integration field dynamic flags have been reset");
}

template <size_t SECTOR_SIDE_LENGTH>
void IntegrationField<SECTOR_SIDE_LENGTH>::update_neighbor(size_t idx,
                                          cost_t cell_cost,
                                          integrated_cost_t integrated_cost,
                                          std::vector<size_t> &wave) {
	ENSURE(cell_cost > COST_INIT, "cost field cell value must be non-zero");

	// Check if the cell is impassable
	// then we don't need to update the integration field
	if (cell_cost == COST_IMPASSABLE) {
		return;
	}

	auto cost = integrated_cost + cell_cost;
	if (cost < this->cells[idx].cost) {
		// If the new integration value is smaller than the current one,
		// update the cell and add it to the open list
		this->cells[idx].cost = cost;

		wave.push_back(idx);
	}
}

template <size_t SECTOR_SIDE_LENGTH>
std::vector<std::pair<int, int>> IntegrationField<SECTOR_SIDE_LENGTH>::get_los_corners(const std::shared_ptr<CostField<SECTOR_SIDE_LENGTH>> &cost_field,
                                                                      const coord::tile_delta &target,
                                                                      const coord::tile_delta &blocker) {
	std::vector<std::pair<int, int>> corners;

	// Get the cost of the blocking cell's neighbors

	// Set all costs to IMPASSABLE at the beginning
	auto top_cost = COST_IMPASSABLE;
	auto left_cost = COST_IMPASSABLE;
	auto bottom_cost = COST_IMPASSABLE;
	auto right_cost = COST_IMPASSABLE;

	std::pair<int, int> top_left{blocker.ne, blocker.se};
	std::pair<int, int> top_right{blocker.ne + 1, blocker.se};
	std::pair<int, int> bottom_left{blocker.ne, blocker.se + 1};
	std::pair<int, int> bottom_right{blocker.ne + 1, blocker.se + 1};

	// Get neighbor costs (if they exist)
	if (blocker.se > 0) {
		top_cost = cost_field->get_cost(blocker.ne, blocker.se - 1);
	}
	if (blocker.ne > 0) {
		left_cost = cost_field->get_cost(blocker.ne - 1, blocker.se);
	}
	if (static_cast<size_t>(blocker.se) < SECTOR_SIDE_LENGTH - 1) {
		bottom_cost = cost_field->get_cost(blocker.ne, blocker.se + 1);
	}
	if (static_cast<size_t>(blocker.ne) < SECTOR_SIDE_LENGTH - 1) {
		right_cost = cost_field->get_cost(blocker.ne + 1, blocker.se);
	}

	// Check which corners are blocking LOS
	// TODO: Currently super complicated and could likely be optimized
	if (blocker.ne == target.ne) {
		// blocking cell is parallel to target on y-axis
		if (blocker.se < target.se) {
			if (left_cost == COST_MIN) {
				// top
				corners.push_back(bottom_left);
			}
			if (right_cost == COST_MIN) {
				// top
				corners.push_back(bottom_right);
			}
		}
		else {
			if (left_cost == COST_MIN) {
				// bottom
				corners.push_back(top_left);
			}
			if (right_cost == COST_MIN) {
				// bottom
				corners.push_back(top_right);
			}
		}
	}
	else if (blocker.se == target.se) {
		// blocking cell is parallel to target on x-axis
		if (blocker.ne < target.ne) {
			if (top_cost == COST_MIN) {
				// right
				corners.push_back(top_right);
			}
			if (bottom_cost == COST_MIN) {
				// right
				corners.push_back(bottom_right);
			}
		}
		else {
			if (top_cost == COST_MIN) {
				// left
				corners.push_back(top_left);
			}
			if (bottom_cost == COST_MIN) {
				// left
				corners.push_back(bottom_left);
			}
		}
	}
	else {
		// blocking cell is diagonal to target on
		if (blocker.ne < target.ne) {
			if (blocker.se < target.se) {
				// top and right
				if (top_cost == COST_MIN and right_cost == COST_MIN) {
					// right
					corners.push_back(top_right);
				}
				if (left_cost == COST_MIN and bottom_cost == COST_MIN) {
					// bottom
					corners.push_back(bottom_left);
				}
			}
			else {
				// bottom and right
				if (bottom_cost == COST_MIN and right_cost == COST_MIN) {
					// right
					corners.push_back(bottom_right);
				}
				if (left_cost == COST_MIN and top_cost == COST_MIN) {
					// top
					corners.push_back(top_left);
				}
			}
		}
		else {
			if (blocker.se < target.se) {
				// top and left
				if (top_cost == COST_MIN and left_cost == COST_MIN) {
					// left
					corners.push_back(top_left);
				}
				if (right_cost == COST_MIN and bottom_cost == COST_MIN) {
					// bottom
					corners.push_back(bottom_right);
				}
			}
			else {
				// bottom and left
				if (bottom_cost == COST_MIN and left_cost == COST_MIN) {
					// left
					corners.push_back(bottom_left);
				}
				if (right_cost == COST_MIN and top_cost == COST_MIN) {
					// top
					corners.push_back(top_right);
				}
			}
		}
	}

	return corners;
}

template <size_t SECTOR_SIDE_LENGTH>
std::vector<size_t> IntegrationField<SECTOR_SIDE_LENGTH>::bresenhams_line(const coord::tile_delta &target,
                                                         int corner_x,
                                                         int corner_y) {
	std::vector<size_t> cells;

	// cell coordinates
	// these have to be offset depending on the line direction
	auto cell_x = corner_x;
	auto cell_y = corner_y;

	// field edge boundary
	int boundary = SECTOR_SIDE_LENGTH;

	// target coordinates
	// offset by 0.5 to get the center of the cell
	double tx = target.ne + 0.5;
	double ty = target.se + 0.5;

	// slope of the line
	double dx = std::abs(tx - corner_x);
	double dy = std::abs(ty - corner_y);
	auto m = dy / dx;

	// error margin for the line
	// if the error is greater than 1.0, we have to move in the y direction
	auto error = m;

	// Check which direction the line is going
	if (corner_x < tx) {
		if (corner_y < ty) { // left and up
			cell_y -= 1;
			cell_x -= 1;
			while (cell_x >= 0 and cell_y >= 0) {
				cells.push_back(cell_x + cell_y * SECTOR_SIDE_LENGTH);
				if (error > 1.0) {
					cell_y -= 1;
					error -= 1.0;
				}
				else {
					cell_x -= 1;
					error += m;
				}
			}
		}

		else { // left and down
			cell_x -= 1;
			while (cell_x >= 0 and cell_y < boundary) {
				cells.push_back(cell_x + cell_y * SECTOR_SIDE_LENGTH);
				if (error > 1.0) {
					cell_y += 1;
					error -= 1.0;
				}
				else {
					cell_x -= 1;
					error += m;
				}
			}
		}
	}
	else {
		if (corner_y < ty) { // right and up
			cell_y -= 1;
			while (cell_x < boundary and cell_y >= 0) {
				cells.push_back(cell_x + cell_y * SECTOR_SIDE_LENGTH);
				if (error > 1.0) {
					cell_y -= 1;
					error -= 1.0;
				}
				else {
					cell_x += 1;
					error += m;
				}
			}
		}
		else { // right and down
			while (cell_x < boundary and cell_y < boundary) {
				cells.push_back(cell_x + cell_y * SECTOR_SIDE_LENGTH);
				if (error > 1.0) {
					cell_y += 1;
					error -= 1.0;
				}
				else {
					cell_x += 1;
					error += m;
				}
			}
		}
	}

	return cells;
}

} // namespace path
} // namespace openage
