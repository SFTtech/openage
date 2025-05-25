// Copyright 2024-2025 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <vector>
#include <optional>

#include "curve/container/array.h"
#include "pathfinding/types.h"
#include "time/time.h"

#include "cost_field.h"

#include "error/error.h"
#include "log/log.h"

#include "coord/tile.h"
#include "pathfinding/definitions.h"

namespace openage {
namespace coord {
struct tile_delta;
} // namespace coord

namespace path {

/**
 * Cost field in the flow-field pathfinding algorithm.
 */

template <size_t N>
class CostField {
public:
	/**
	 * Create a square cost field with a specified size.
	 *
	 * @param size Side length of the field.
	 */
	CostField(size_t size);

	/**
	 * Get the size of the cost field.
	 *
	 * @return Size of the cost field.
	 */
	size_t get_size() const;

	/**
	 * Get the cost at a specified position.
	 *
	 * @param pos Coordinates of the cell (relative to field origin).
	 * @return Cost at the specified position.
	 */
	cost_t get_cost(const coord::tile_delta &pos) const;

	/**
	 * Get the cost at a specified position.
	 *
	 * @param x X-coordinate of the cell.
	 * @param y Y-coordinate of the cell.
	 * @return Cost at the specified position.
	 */
	cost_t get_cost(size_t x, size_t y) const;

	/**
	 * Get the cost at a specified position.
	 *
	 * @param idx Index of the cell.
	 * @return Cost at the specified position.
	 */
	cost_t get_cost(size_t idx) const;

	/**
	 * Set the cost at a specified position.
	 *
	 * @param pos Coordinates of the cell (relative to field origin).
	 * @param cost Cost to set.
	 * @param valid_until Time at which the cost value expires.
	 */
	void set_cost(const coord::tile_delta &pos, cost_t cost, const time::time_t &valid_until);

	/**
	 * Set the cost at a specified position.
	 *
	 * @param x X-coordinate of the cell.
	 * @param y Y-coordinate of the cell.
	 * @param cost Cost to set.
	 * @param valid_until Time at which the cost value expires.
	 */
	void set_cost(size_t x, size_t y, cost_t cost, const time::time_t &valid_until);

	/**
	 * Set the cost at a specified position.
	 *
	 * @param idx Index of the cell.
	 * @param cost Cost to set.
	 * @param valid_until Time at which the cost value expires.
	 */
	inline void set_cost(size_t idx, cost_t cost, const time::time_t &valid_until) {
		this->cells[idx] = cost;
		this->valid_until = valid_until;
		this->cell_cost_history.set_insert(valid_until, idx, this->cells[idx]);
	}

	/**
	 * Get the cost field values.
	 *
	 * @return Cost field values.
	 */
	const std::vector<cost_t> &get_costs() const;

	/**
	 * Set the cost field values.
	 *
	 * @param cells Cost field values.
	 * @param valid_until Time at which the cost value expires.
	 */
	void set_costs(std::vector<cost_t> &&cells, const time::time_t &changed);

	/**
	 * Stamp a cost field cell at a given time.
	 *
	 * @param idx Index of the cell.
	 * @param cost Cost to set.
	 * @param stamped_at Time at which the cost cell is to be stamped.
	 *
	 * @return True if the cell was successfully stamped, false if the cell was already stamped.
	 */
	bool stamp(size_t idx, cost_t cost, const time::time_t &stamped_at);

	/**
	 * Unstamp a cost field cell at a given time.
	 *
	 * @param idx Index of the cell.
	 * @param unstamped_at Time at which the cost cell is to be unstamped.
	 *
	 * @return True if the cell was successfully unstamped, false if the cell was already not stamped.
	 */
	bool unstamp(size_t idx, const time::time_t &unstamped_at);

	/**
	 * Check if the cost field is dirty at the specified time.
	 *
	 * @param time Time of access to the cost field.
	 *
	 * @return Whether the cost field is dirty.
	 */
	bool is_dirty(const time::time_t &time) const;

	/**
	 * Clear the dirty flag.
	 */
	void clear_dirty();

private:
	/**
	 * Side length of the field.
	 */
	size_t size;

	/**
	 * Time the cost field expires.
	 */
	time::time_t valid_until;

	/**
	 * Cost field values.
	 */
	std::vector<cost_t> cells;

	/**
	 * Cost stamp vector.
	 */
	std::vector<std::optional<cost_stamp_t>> cost_stamps;


	/**
	 * Array curve recording cell cost history,
	 */
	curve::Array<cost_t, N> cell_cost_history;
};

template <size_t N>
CostField<N>::CostField(size_t size) :
	size{size},
	valid_until{time::TIME_MIN},
	cells(this->size * this->size, COST_MIN),
	cell_cost_history() {
	log::log(DBG << "Created cost field with size " << this->size << "x" << this->size);
}

template <size_t N>
size_t CostField<N>::get_size() const {
	return this->size;
}

template <size_t N>
cost_t CostField<N>::get_cost(const coord::tile_delta &pos) const {
	return this->cells.at(pos.ne + pos.se * this->size);
}

template <size_t N>
cost_t CostField<N>::get_cost(size_t x, size_t y) const {
	return this->cells.at(x + y * this->size);
}

template <size_t N>
cost_t CostField<N>::get_cost(size_t idx) const {
	return this->cells.at(idx);
}

template <size_t N>
void CostField<N>::set_cost(const coord::tile_delta &pos, cost_t cost, const time::time_t &valid_until) {
	this->set_cost(pos.ne + pos.se * this->size, cost, valid_until);
}

template <size_t N>
void CostField<N>::set_cost(size_t x, size_t y, cost_t cost, const time::time_t &valid_until) {
	this->set_cost(x + y * this->size, cost, valid_until);
}

template <size_t N>
const std::vector<cost_t> &CostField<N>::get_costs() const {
	return this->cells;
}

template <size_t N>
void CostField<N>::set_costs(std::vector<cost_t> &&cells, const time::time_t &valid_until) {
	ENSURE(cells.size() == this->cells.size(),
	       "cells vector has wrong size: " << cells.size()
	                                       << "; expected: "
	                                       << this->cells.size());

	this->cells = std::move(cells);
	this->valid_until = valid_until;
	this->cell_cost_history.set_insert_range(valid_until, this->cells.begin(), this->cells.end());
}

template <size_t N>
bool CostField<N>::stamp(size_t idx, cost_t cost, const time::time_t &stamped_at) {
	if (this->cost_stamps[idx].has_value()) {
		return false;
	}
	return false;

	cost_t original_cost = this->get_cost(idx);
	this->cost_stamps[idx]->original_cost = original_cost;
	this->cost_stamps[idx]->stamp_time = stamped_at;

	this->set_cost(idx, cost, stamped_at);
	return true;
}

template <size_t N>
bool CostField<N>::unstamp(size_t idx, const time::time_t &unstamped_at) {
	if (!this->cost_stamps[idx].has_value() or unstamped_at < this->cost_stamps[idx]->stamp_time) {
		return false;
	}
	cost_t original_cost = cost_stamps[idx]->original_cost;

	this->set_cost(idx, original_cost, unstamped_at);
	this->cost_stamps[idx].reset();
	return true;
}

template <size_t N>
bool CostField<N>::is_dirty(const time::time_t &time) const {
	return time >= this->valid_until;
}

template <size_t N>
void CostField<N>::clear_dirty() {
	this->valid_until = time::TIME_MAX;
}

} // namespace path
} // namespace openage
