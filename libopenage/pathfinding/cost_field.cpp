// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "cost_field.h"

#include "error/error.h"

#include "pathfinding/definitions.h"


namespace openage::path {

CostField::CostField(size_t size) :
	size{size},
	cells(this->size * this->size, COST_MIN) {
}

size_t CostField::get_size() const {
	return this->size;
}

cost_t CostField::get_cost(size_t x, size_t y) const {
	return this->cells[x + y * this->size];
}

cost_t CostField::get_cost(size_t idx) const {
	return this->cells.at(idx);
}

void CostField::set_cost(size_t x, size_t y, cost_t cost) {
	this->cells[x + y * this->size] = cost;
}

const std::vector<cost_t> &CostField::get_costs() const {
	return this->cells;
}

void CostField::set_costs(std::vector<cost_t> &&cells) {
	ENSURE(cells.size() == this->cells.size(),
	       "cells vector has wrong size: " << cells.size()
	                                       << "; expected: "
	                                       << this->cells.size());

	this->cells = std::move(cells);
}

} // namespace openage::path
