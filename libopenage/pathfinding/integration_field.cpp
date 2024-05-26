// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "integration_field.h"

#include <cmath>

#include "error/error.h"
#include "log/log.h"

#include "coord/tile.h"
#include "pathfinding/cost_field.h"
#include "pathfinding/definitions.h"
#include "pathfinding/portal.h"


namespace openage::path {

IntegrationField::IntegrationField(size_t size) :
	size{size},
	cells(this->size * this->size, INTEGRATE_INIT) {
	log::log(DBG << "Created integration field with size " << this->size << "x" << this->size);
}

size_t IntegrationField::get_size() const {
	return this->size;
}

const integrated_t &IntegrationField::get_cell(const coord::tile_delta &pos) const {
	return this->cells.at(pos.ne + pos.se * this->size);
}

const integrated_t &IntegrationField::get_cell(size_t x, size_t y) const {
	return this->cells.at(x + y * this->size);
}

const integrated_t &IntegrationField::get_cell(size_t idx) const {
	return this->cells.at(idx);
}

std::vector<size_t> IntegrationField::integrate_los(const std::shared_ptr<CostField> &cost_field,
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
	auto entry_start = portal->get_entry_start(other_sector_id);
	auto entry_end = portal->get_entry_end(other_sector_id);

	auto x_diff = exit_start.ne - entry_start.ne;
	auto y_diff = exit_start.se - entry_start.se;

	for (auto y = exit_start.se; y <= exit_end.se; ++y) {
		for (auto x = exit_start.ne; x <= exit_end.ne; ++x) {
			// every portal cell is a target cell
			auto target_idx = x + y * this->size;

			auto source_idx = x - x_diff + (y - y_diff) * this->size;

			// Set the cost of all target cells to the start value
			this->cells[target_idx].cost = INTEGRATED_COST_START;
			this->cells[target_idx].flags = this->cells[source_idx].flags;

			start_cells.push_back(target_idx);
		}
	}

	// TODO: Call main LOS integration function
}

std::vector<size_t> IntegrationField::integrate_los(const std::shared_ptr<CostField> &cost_field,
                                                    const coord::tile_delta &target) {
	ENSURE(cost_field->get_size() == this->get_size(),
	       "cost field size "
	           << cost_field->get_size() << "x" << cost_field->get_size()
	           << " does not match integration field size "
	           << this->get_size() << "x" << this->get_size());

	// Store the wavefront_blocked cells
	std::vector<size_t> wavefront_blocked;

	// Target cell index
	auto target_idx = target.ne + target.se * this->size;

	// Cells that still have to be visited by the current wave
	std::deque<size_t> current_wave;

	// Cells that have to be visited in the next wave
	std::deque<size_t> next_wave;

	// Cost of the current wave
	integrated_cost_t cost = INTEGRATED_COST_START;

	// Move outwards from the target cell, updating the integration field
	current_wave.push_back(target_idx);
	do {
		while (!current_wave.empty()) {
			auto idx = current_wave.front();
			current_wave.pop_front();

			if (this->cells[idx].flags & INTEGRATE_FOUND_MASK) {
				// Skip cells that are already in the line of sight
				continue;
			}
			else if (this->cells[idx].flags & INTEGRATE_WAVEFRONT_BLOCKED_MASK) {
				// Stop at cells that are blocked by a LOS corner
				this->cells[idx].cost = cost - 1 + cost_field->get_cost(idx);
				this->cells[idx].flags |= INTEGRATE_FOUND_MASK;
				continue;
			}

			// Add the current cell to the found cells
			this->cells[idx].flags |= INTEGRATE_FOUND_MASK;

			// Get the x and y coordinates of the current cell
			auto x = idx % this->size;
			auto y = idx / this->size;

			// Get the cost of the current cell
			auto cell_cost = cost_field->get_cost(idx);

			if (cell_cost > COST_MIN) {
				// cell blocks line of sight
				// and we have to check for corners
				if (cell_cost != COST_IMPASSABLE) {
					// Add the current cell to the blocked wavefront if it's not a wall
					wavefront_blocked.push_back(idx);
					this->cells[idx].cost = cost - 1 + cost_field->get_cost(idx);
					// TODO: this->cells[idx].flags |= INTEGRATE_WAVEFRONT_BLOCKED_MASK;
				}

				// check each neighbor for a corner
				auto corners = this->get_los_corners(cost_field, target, coord::tile_delta(x, y));
				for (auto &corner : corners) {
					// draw a line from the corner to the edge of the field
					// to get the cells blocked by the corner
					auto blocked_cells = this->bresenhams_line(target, corner.first, corner.second);
					for (auto &blocked_idx : blocked_cells) {
						if (cost_field->get_cost(blocked_idx) > COST_MIN) {
							// stop if blocked_idx is impassable
							break;
						}
						// set the blocked flag for the cell
						this->cells[blocked_idx].flags |= INTEGRATE_WAVEFRONT_BLOCKED_MASK;

						// clear los flag if it was set
						this->cells[blocked_idx].flags &= ~INTEGRATE_LOS_MASK;

						wavefront_blocked.push_back(blocked_idx);
					}
				}
				continue;
			}

			// The cell is in the line of sight at min cost
			// Set the LOS flag and cost
			this->cells[idx].cost = cost;
			this->cells[idx].flags |= INTEGRATE_LOS_MASK;

			// Search the neighbors of the current cell
			if (y > 0) {
				next_wave.push_back(idx - this->size);
			}
			if (x > 0) {
				next_wave.push_back(idx - 1);
			}
			if (y < this->size - 1) {
				next_wave.push_back(idx + this->size);
			}
			if (x < this->size - 1) {
				next_wave.push_back(idx + 1);
			}
		}

		// increment the cost and advance the wavefront outwards
		cost += 1;
		current_wave.swap(next_wave);
		next_wave.clear();
	}
	while (!current_wave.empty());

	return wavefront_blocked;
}

void IntegrationField::integrate_cost(const std::shared_ptr<CostField> &cost_field,
                                      const coord::tile_delta &target) {
	ENSURE(cost_field->get_size() == this->get_size(),
	       "cost field size "
	           << cost_field->get_size() << "x" << cost_field->get_size()
	           << " does not match integration field size "
	           << this->get_size() << "x" << this->get_size());

	// Target cell index
	auto target_idx = target.ne + target.se * this->size;

	// Move outwards from the target cell, updating the integration field
	this->cells[target_idx].cost = INTEGRATED_COST_START;
	this->cells[target_idx].flags |= INTEGRATE_TARGET_MASK;
	this->integrate_cost(cost_field, {target_idx});
}

void IntegrationField::integrate_cost(const std::shared_ptr<CostField> &cost_field,
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
			auto target_idx = x + y * this->size;

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

void IntegrationField::integrate_cost(const std::shared_ptr<CostField> &cost_field,
                                      std::vector<size_t> &&start_cells) {
	// Lookup table for cells that are in the open list
	std::unordered_set<size_t> in_list;
	in_list.reserve(this->size * this->size);

	// Cells that still have to be visited
	// they may be visited multiple times
	std::deque<size_t> open_list;

	// Stores neighbors of the current cell
	std::vector<size_t> neighbors;
	neighbors.reserve(4);

	// Move outwards from the wavefront, updating the integration field
	open_list.insert(open_list.end(), start_cells.begin(), start_cells.end());
	while (!open_list.empty()) {
		auto idx = open_list.front();
		open_list.pop_front();

		// Get the x and y coordinates of the current cell
		auto x = idx % this->size;
		auto y = idx / this->size;

		auto integrated_current = this->cells.at(idx).cost;

		// Get the neighbors of the current cell
		if (y > 0) {
			neighbors.push_back(idx - this->size);
		}
		if (x > 0) {
			neighbors.push_back(idx - 1);
		}
		if (y < this->size - 1) {
			neighbors.push_back(idx + this->size);
		}
		if (x < this->size - 1) {
			neighbors.push_back(idx + 1);
		}

		// Update the integration field of the neighboring cells
		for (auto &neighbor_idx : neighbors) {
			this->update_neighbor(neighbor_idx,
			                      cost_field->get_cost(neighbor_idx),
			                      integrated_current,
			                      open_list,
			                      in_list);
		}

		// Clear the neighbors vector
		neighbors.clear();

		// Remove the current cell from the open list lookup table
		in_list.erase(idx);
	}
}

const std::vector<integrated_t> &IntegrationField::get_cells() const {
	return this->cells;
}

void IntegrationField::reset() {
	std::fill(this->cells.begin(), this->cells.end(), INTEGRATE_INIT);

	log::log(DBG << "Integration field has been reset");
}

void IntegrationField::reset_dynamic_flags() {
	integrated_flags_t mask = 0xFF & ~(INTEGRATE_LOS_MASK | INTEGRATE_WAVEFRONT_BLOCKED_MASK);
	for (integrated_t &cell : this->cells) {
		cell.flags = cell.flags & mask;
	}

	log::log(DBG << "Integration field dynamic flags have been reset");
}

void IntegrationField::update_neighbor(size_t idx,
                                       cost_t cell_cost,
                                       integrated_cost_t integrated_cost,
                                       std::deque<size_t> &open_list,
                                       std::unordered_set<size_t> &in_list) {
	ENSURE(cell_cost > COST_INIT, "cost field cell value must be non-zero");

	// Check if the cell is impassable
	// then we don't need to update the integration field
	if (cell_cost == COST_IMPASSABLE) {
		return;
	}

	auto cost = integrated_cost + cell_cost;
	if (cost < this->cells.at(idx).cost) {
		// If the new integration value is smaller than the current one,
		// update the cell and add it to the open list
		this->cells[idx].cost = cost;

		if (not in_list.contains(idx)) {
			in_list.insert(idx);
			open_list.push_back(idx);
		}
	}
}

std::vector<std::pair<int, int>> IntegrationField::get_los_corners(const std::shared_ptr<CostField> &cost_field,
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
	if (static_cast<size_t>(blocker.se) < this->size - 1) {
		bottom_cost = cost_field->get_cost(blocker.ne, blocker.se + 1);
	}
	if (static_cast<size_t>(blocker.ne) < this->size - 1) {
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

std::vector<size_t> IntegrationField::bresenhams_line(const coord::tile_delta &target,
                                                      int corner_x,
                                                      int corner_y) {
	std::vector<size_t> cells;

	// cell coordinates
	// these have to be offset depending on the line direction
	auto cell_x = corner_x;
	auto cell_y = corner_y;

	// field edge boundary
	int boundary = this->size;

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
				cells.push_back(cell_x + cell_y * this->size);
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
				cells.push_back(cell_x + cell_y * this->size);
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
				cells.push_back(cell_x + cell_y * this->size);
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
				cells.push_back(cell_x + cell_y * this->size);
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


} // namespace openage::path
