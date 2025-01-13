// Copyright 2024-2025 the openage authors. See copying.md for legal info.

#include "cost_field.h"

#include "error/error.h"
#include "log/log.h"

#include "coord/tile.h"
#include "pathfinding/definitions.h"


namespace openage::path {

CostField::CostField(size_t size) :
	size{size},
	valid_until{time::TIME_MIN},
	cells(this->size * this->size, COST_MIN) {
	log::log(DBG << "Created cost field with size " << this->size << "x" << this->size);
}

size_t CostField::get_size() const {
	return this->size;
}

cost_t CostField::get_cost(const coord::tile_delta &pos) const {
	return this->cells.at(pos.ne + pos.se * this->size);
}

cost_t CostField::get_cost(size_t x, size_t y) const {
	return this->cells.at(x + y * this->size);
}

cost_t CostField::get_cost(size_t idx) const {
	return this->cells.at(idx);
}

void CostField::set_cost(const coord::tile_delta &pos, cost_t cost, const time::time_t &valid_until) {
	this->set_cost(pos.ne + pos.se * this->size, cost, valid_until);
}

void CostField::set_cost(size_t x, size_t y, cost_t cost, const time::time_t &valid_until) {
	this->set_cost(x + y * this->size, cost, valid_until);
}

const std::vector<cost_t> &CostField::get_costs() const {
	return this->cells;
}

void CostField::set_costs(std::vector<cost_t> &&cells, const time::time_t &valid_until) {
	ENSURE(cells.size() == this->cells.size(),
	       "cells vector has wrong size: " << cells.size()
	                                       << "; expected: "
	                                       << this->cells.size());

	this->cells = std::move(cells);
	this->valid_until = valid_until;
}

bool CostField::is_dirty(const time::time_t &time) const {
	return time >= this->valid_until;
}

void CostField::clear_dirty() {
	this->valid_until = time::TIME_MAX;
}

} // namespace openage::path
