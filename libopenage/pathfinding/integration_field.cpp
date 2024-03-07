// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "integration_field.h"

#include <cmath>

#include "error/error.h"
#include "log/log.h"

#include "pathfinding/cost_field.h"
#include "pathfinding/definitions.h"


namespace openage::path {

IntegrationField::IntegrationField(size_t size) :
	size{size},
	cells(this->size * this->size, INTEGRATE_INIT) {
	log::log(DBG << "Created integration field with size " << this->size << "x" << this->size);
}

size_t IntegrationField::get_size() const {
	return this->size;
}

const integrate_t &IntegrationField::get_cell(size_t x, size_t y) const {
	return this->cells.at(x + y * this->size);
}

const integrate_t &IntegrationField::get_cell(size_t idx) const {
	return this->cells.at(idx);
}

std::vector<size_t> IntegrationField::integrate_los(const std::shared_ptr<CostField> &cost_field,
                                                    size_t target_x,
                                                    size_t target_y) {
	ENSURE(cost_field->get_size() == this->get_size(),
	       "cost field size "
	           << cost_field->get_size() << "x" << cost_field->get_size()
	           << " does not match integration field size "
	           << this->get_size() << "x" << this->get_size());

	// Store the wavefront cells
	std::vector<size_t> wavefront;

	// Target cell index
	auto target_idx = target_x + target_y * this->size;

	// Lookup table for cells that have been found
	std::unordered_set<size_t> found;
	found.reserve(this->size * this->size);

	// Cells that still have to be visited
	std::deque<size_t> open_list;

	// Move outwards from the target cell, updating the integration field
	open_list.push_back(target_idx);
	while (!open_list.empty()) {
		auto idx = open_list.front();
		open_list.pop_front();

		if (found.contains(idx)) {
			// Skip cells that have already been found
			continue;
		}
		else if (this->cells[idx].flags & INTEGRATE_WAVEFRONT_BLOCKED_MASK) {
			// Skip cells that are blocked by a LOS corner
			continue;
		}

		// Add the current cell to the found cells
		found.insert(idx);

		// Get the x and y coordinates of the current cell
		auto x = idx % this->size;
		auto y = idx / this->size;

		// Get the cost of the current cell
		auto cell_cost = cost_field->get_cost(idx);

		if (cell_cost > COST_MIN) {
			// check each neighbor for a corner
			auto corners = this->get_los_corners(cost_field, target_x, target_y, x, y);

			for (auto &corner : corners) {
				auto blocked_cells = this->bresenhams_line(target_x, target_y, corner.first, corner.second);
				for (auto &blocked_idx : blocked_cells) {
					this->cells[blocked_idx].flags |= INTEGRATE_WAVEFRONT_BLOCKED_MASK;
				}
				wavefront.insert(wavefront.end(), blocked_cells.begin(), blocked_cells.end());
			}

			continue;
		}

		// Set the LOS flag of the current cell
		this->cells[idx].flags |= INTEGRATE_LOS_MASK;

		// Search the neighbors of the current cell
		if (y > 0) {
			open_list.push_back(idx - this->size);
		}
		if (x > 0) {
			open_list.push_back(idx - 1);
		}
		if (y < this->size - 1) {
			open_list.push_back(idx + this->size);
		}
		if (x < this->size - 1) {
			open_list.push_back(idx + 1);
		}
	}

	return wavefront;
}

void IntegrationField::integrate_cost(const std::shared_ptr<CostField> &cost_field,
                                      size_t target_x,
                                      size_t target_y) {
	ENSURE(cost_field->get_size() == this->get_size(),
	       "cost field size "
	           << cost_field->get_size() << "x" << cost_field->get_size()
	           << " does not match integration field size "
	           << this->get_size() << "x" << this->get_size());

	// Target cell index
	auto target_idx = target_x + target_y * this->size;

	// Move outwards from the target cell, updating the integration field
	this->cells[target_idx].cost = INTEGRATED_COST_START;
	this->integrate_cost(cost_field, {target_idx});
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

const std::vector<integrate_t> &IntegrationField::get_cells() const {
	return this->cells;
}

void IntegrationField::reset() {
	for (auto &cell : this->cells) {
		cell = INTEGRATE_INIT;
	}
	log::log(DBG << "Integration field has been reset");
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

	// if (this->cells.at(idx).flags & INTEGRATE_LOS_MASK) {
	// 	// If the cell is part of the LOS, we don't need to update it
	// 	return;
	// }

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

std::vector<std::pair<size_t, size_t>> IntegrationField::get_los_corners(const std::shared_ptr<CostField> &cost_field,
                                                                         size_t target_x,
                                                                         size_t target_y,
                                                                         size_t blocker_x,
                                                                         size_t blocker_y) {
	std::vector<std::pair<size_t, size_t>> corners;

	// Get the cost of the blocking cell's neighbors

	// Set all costs to IMPASSABLE at the beginning
	auto top_cost = COST_IMPASSABLE;
	auto left_cost = COST_IMPASSABLE;
	auto bottom_cost = COST_IMPASSABLE;
	auto right_cost = COST_IMPASSABLE;

	std::pair<size_t, size_t> top_left{blocker_x, blocker_y};
	std::pair<size_t, size_t> top_right{blocker_x + 1, blocker_y};
	std::pair<size_t, size_t> bottom_left{blocker_x, blocker_y + 1};
	std::pair<size_t, size_t> bottom_right{blocker_x + 1, blocker_y + 1};

	// Get neighbor costs (if they exist)
	if (blocker_y > 0) {
		top_cost = cost_field->get_cost(blocker_x, blocker_y - 1);
	}
	if (blocker_x > 0) {
		left_cost = cost_field->get_cost(blocker_x - 1, blocker_y);
	}
	if (blocker_y < this->size - 1) {
		bottom_cost = cost_field->get_cost(blocker_x, blocker_y + 1);
	}
	if (blocker_x < this->size - 1) {
		right_cost = cost_field->get_cost(blocker_x + 1, blocker_y);
	}

	// Check which corners are blocking LOS
	// TODO: Currently super complicated and could likely be optimized
	if (blocker_x == target_x) {
		// blocking cell is parallel to target on y-axis
		if (blocker_y < target_y) {
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
	else if (blocker_y == target_y) {
		// blocking cell is parallel to target on x-axis
		if (blocker_x < target_x) {
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
		if (blocker_x < target_x) {
			if (blocker_y < target_y) {
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
			if (blocker_y < target_y) {
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

std::vector<size_t> IntegrationField::bresenhams_line(int target_x,
                                                      int target_y,
                                                      int corner_x,
                                                      int corner_y) {
	std::vector<size_t> cells;

	auto x = corner_x;
	auto y = corner_y;
	double tx = target_x + 0.5;
	double ty = target_y + 0.5;
	double dx = std::abs(tx - corner_x);
	double dy = std::abs(ty - corner_y);
	auto m = dy / dx;

	auto error = 0.5;

	// Check which direction the line is going
	if (corner_x < tx) {
		if (corner_y < ty) {
			// left and up
			y -= 1;
			while (x > 0 and y > 0) {
				if (error > 1.0) {
					y -= 1;
					error -= 1.0;
				}
				else {
					x -= 1;
					error += m;
				}
				cells.push_back(x + y * this->size);
			}
		}
		else {
			// left and down
			while (x > 0 and y < this->size - 1) {
				if (error > 1.0) {
					y += 1;
					error -= 1.0;
				}
				else {
					x -= 1;
					error += m;
				}
				cells.push_back(x + y * this->size);
			}
		}
	}
	else {
		if (corner_y < ty) {
			// right and up
			x -= 1;
			y -= 1;
			while (x < this->size - 1 and y > 0) {
				if (error > 1.0) {
					y -= 1;
					error -= 1.0;
				}
				else {
					x += 1;
					error += m;
				}
				cells.push_back(x + y * this->size);
			}
		}
		else {
			// right and down
			x -= 1;
			while (x < this->size - 1 and y < this->size - 1) {
				if (error > 1.0) {
					y += 1;
					error -= 1.0;
				}
				else {
					x += 1;
					error += m;
				}
				cells.push_back(x + y * this->size);
			}
		}
	}

	return cells;
}


} // namespace openage::path
