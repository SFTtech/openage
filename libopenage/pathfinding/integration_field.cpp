// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "integration_field.h"

#include <deque>
#include <unordered_set>

#include "error/error.h"

#include "pathfinding/cost_field.h"
#include "pathfinding/definitions.h"


namespace openage::path {

/**
 * Update the open list for a neighbour index during integration field
 * calculation (subroutine of integrate(..)).
 *
 * Checks whether:
 *
 * 1. the neighbour index has not already been found
 * 2. the neighbour index is reachable
 *
 * If both conditions are met, the neighbour index is added to the open list.
 *
 * @param idx Index of the cell to update.
 * @param integrate_cost Integrated cost of the cell.
 * @param open_list Cells that still have to be visited.
 * @param found Cells that have been found.
 */
void update_list(size_t idx,
                 integrate_t integrate_cost,
                 std::deque<size_t> &open_list,
                 std::unordered_set<size_t> &found) {
	if (not found.contains(idx)) {
		found.insert(idx);

		if (integrate_cost != INTEGRATE_UNREACHABLE) {
			open_list.push_back(idx);
		}
	}
}


IntegrationField::IntegrationField(size_t size) :
	size{size},
	cells(this->size * this->size, INTEGRATE_UNREACHABLE) {
}

size_t IntegrationField::get_size() const {
	return this->size;
}

void IntegrationField::integrate(const std::shared_ptr<CostField> &cost_field,
                                 size_t target_x,
                                 size_t target_y) {
	ENSURE(cost_field->get_size() == this->get_size(),
	       "cost field size "
	           << cost_field->get_size() << "x" << cost_field->get_size()
	           << " does not match integration field size "
	           << this->get_size() << "x" << this->get_size());

	// Reset the integration field
	this->reset();

	// Target cell index
	auto target_idx = target_x + target_y * this->size;

	// Cells that have been found
	std::unordered_set<size_t> found;
	found.reserve(this->size * this->size);
	// Cells that still have to be visited
	std::deque<size_t> open_list;

	// Move outwards from the target cell, updating the integration field
	this->cells[target_idx] = INTEGRATE_START;
	open_list.push_back(target_idx);
	while (!open_list.empty()) {
		auto idx = open_list.front();
		open_list.pop_front();

		// Get the x and y coordinates of the current cell
		auto x = idx % this->size;
		auto y = idx / this->size;

		auto integrated_current = this->cells[idx];

		// Update the integration field of the 4 neighbouring cells
		if (y > 0) {
			auto up_idx = idx - this->size;
			auto neighbor_cost = this->update_cell(up_idx,
			                                       cost_field->get_cost(up_idx),
			                                       integrated_current);
			update_list(up_idx, neighbor_cost, open_list, found);
		}
		if (x > 0) {
			auto left_idx = idx - 1;
			auto neighbor_cost = this->update_cell(left_idx,
			                                       cost_field->get_cost(left_idx),
			                                       integrated_current);
			update_list(left_idx, neighbor_cost, open_list, found);
		}
		if (y < this->size - 1) {
			auto down_idx = idx + this->size;
			auto neighbor_cost = this->update_cell(down_idx,
			                                       cost_field->get_cost(down_idx),
			                                       integrated_current);
			update_list(down_idx, neighbor_cost, open_list, found);
		}
		if (x < this->size - 1) {
			auto right_idx = idx + 1;
			auto neighbor_cost = this->update_cell(right_idx,
			                                       cost_field->get_cost(right_idx),
			                                       integrated_current);
			update_list(right_idx, neighbor_cost, open_list, found);
		}
	}
}

void IntegrationField::reset() {
	for (auto &cell : this->cells) {
		cell = INTEGRATE_UNREACHABLE;
	}
}

integrate_t IntegrationField::update_cell(size_t idx,
                                          cost_t cell_cost,
                                          cost_t integrate_cost) {
	ENSURE(cell_cost > COST_INIT, "cost field cell value must be non-zero");

	// Check if the cell is impassable.
	if (cell_cost == COST_IMPASSABLE) {
		return INTEGRATE_UNREACHABLE;
	}

	// Update the integration field value of the cell.
	auto integrated = integrate_cost + cell_cost;
	if (integrated < this->cells.at(idx)) {
		this->cells[idx] = integrated;
	}

	return integrated;
}

} // namespace openage::path
