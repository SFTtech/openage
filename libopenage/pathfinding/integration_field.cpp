// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "integration_field.h"

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

void IntegrationField::integrate_cost(const std::shared_ptr<CostField> &cost_field,
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

	// Lookup table for cells that are in the open list
	std::unordered_set<size_t> in_list;
	in_list.reserve(this->size * this->size);

	// Cells that still have to be visited
	// they may be visited multiple times
	std::deque<size_t> open_list;

	// Stores neighbors of the current cell
	std::vector<size_t> neighbors;
	neighbors.reserve(4);

	// Move outwards from the target cell, updating the integration field
	this->cells[target_idx].cost = INTEGRATED_COST_START;
	open_list.push_back(target_idx);
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


} // namespace openage::path
