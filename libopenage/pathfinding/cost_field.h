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

template <size_t SECTOR_SIDE_LENGTH>
class CostField {
public:
	/**
	 * Create a square cost field.
	 */
	CostField(const std::shared_ptr<event::EventLoop> &loop = nullptr, size_t id = 0);

	/**
	 * Get the size of the cost field.
	 *
	 * @return Size of the cost field.
	 */
	constexpr size_t get_size() const;

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
	const std::array<cost_t, SECTOR_SIDE_LENGTH * SECTOR_SIDE_LENGTH> &get_costs() const;

	/**
	 * Set the cost field values.
	 *
	 * @param cells Cost field values.
	 * @param valid_until Time at which the cost value expires.
	 */
	void set_costs(std::array<cost_t, SECTOR_SIDE_LENGTH * SECTOR_SIDE_LENGTH> &&cells, const time::time_t &changed);

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


	const curve::Array<cost_t, SECTOR_SIDE_LENGTH> &get_cost_history() const;

private:
	/**
	 * Time the cost field expires.
	 */
	time::time_t valid_until;

	/**
	 * Cost field values.
	 */
	std::array<cost_t, SECTOR_SIDE_LENGTH * SECTOR_SIDE_LENGTH> cells;

	/**
	 * Cost stamp vector.
	 */
	std::array<std::optional<cost_stamp_t>, SECTOR_SIDE_LENGTH * SECTOR_SIDE_LENGTH> cost_stamps;


	/**
	 * Array curve recording cell cost history,
	 */
	curve::Array<cost_t, SECTOR_SIDE_LENGTH * SECTOR_SIDE_LENGTH> cell_cost_history;
};

template <size_t SECTOR_SIDE_LENGTH>
CostField<SECTOR_SIDE_LENGTH>::CostField(const std::shared_ptr<event::EventLoop> &loop, size_t id) :
	valid_until{time::TIME_MIN},
	cell_cost_history(loop, id) {
	cells.fill(COST_MIN);
	log::log(DBG << "Created cost field with size " << SECTOR_SIDE_LENGTH << "x" << SECTOR_SIDE_LENGTH);
}

template <size_t SECTOR_SIDE_LENGTH>
constexpr size_t CostField<SECTOR_SIDE_LENGTH>::get_size() const {
	return SECTOR_SIDE_LENGTH;
}

template <size_t SECTOR_SIDE_LENGTH>
cost_t CostField<SECTOR_SIDE_LENGTH>::get_cost(const coord::tile_delta &pos) const {
	return this->cells.at(pos.ne + pos.se * SECTOR_SIDE_LENGTH);
}

template <size_t SECTOR_SIDE_LENGTH>
cost_t CostField<SECTOR_SIDE_LENGTH>::get_cost(size_t x, size_t y) const {
	return this->cells.at(x + y * SECTOR_SIDE_LENGTH);
}

template <size_t SECTOR_SIDE_LENGTH>
cost_t CostField<SECTOR_SIDE_LENGTH>::get_cost(size_t idx) const {
	return this->cells.at(idx);
}

template <size_t SECTOR_SIDE_LENGTH>
void CostField<SECTOR_SIDE_LENGTH>::set_cost(const coord::tile_delta &pos, cost_t cost, const time::time_t &valid_until) {
	this->set_cost(pos.ne + pos.se * SECTOR_SIDE_LENGTH, cost, valid_until);
}

template <size_t SECTOR_SIDE_LENGTH>
void CostField<SECTOR_SIDE_LENGTH>::set_cost(size_t x, size_t y, cost_t cost, const time::time_t &valid_until) {
	this->set_cost(x + y * SECTOR_SIDE_LENGTH, cost, valid_until);
}

template <size_t SECTOR_SIDE_LENGTH>
const std::array<cost_t, SECTOR_SIDE_LENGTH * SECTOR_SIDE_LENGTH> &CostField<SECTOR_SIDE_LENGTH>::get_costs() const {
	return this->cells;
}

template <size_t SECTOR_SIDE_LENGTH>
void CostField<SECTOR_SIDE_LENGTH>::set_costs(std::array<cost_t, SECTOR_SIDE_LENGTH * SECTOR_SIDE_LENGTH> &&cells, const time::time_t &valid_until) {
	ENSURE(cells.size() == this->cells.size(),
	       "cells vector has wrong size: " << cells.size()
	                                       << "; expected: "
	                                       << this->cells.size());

	this->cells = std::move(cells);
	this->valid_until = valid_until;
	this->cell_cost_history.set_insert_range(valid_until, this->cells.begin(), this->cells.end());
}

template <size_t SECTOR_SIDE_LENGTH>
bool CostField<SECTOR_SIDE_LENGTH>::stamp(size_t idx, cost_t cost, const time::time_t &stamped_at) {
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

template <size_t SECTOR_SIDE_LENGTH>
bool CostField<SECTOR_SIDE_LENGTH>::unstamp(size_t idx, const time::time_t &unstamped_at) {
	if (not this->cost_stamps[idx].has_value() or unstamped_at < this->cost_stamps[idx]->stamp_time) {
		return false;
	}
	cost_t original_cost = cost_stamps[idx]->original_cost;

	this->set_cost(idx, original_cost, unstamped_at);
	this->cost_stamps[idx].reset();
	return true;
}

template <size_t SECTOR_SIDE_LENGTH>
bool CostField<SECTOR_SIDE_LENGTH>::is_dirty(const time::time_t &time) const {
	return time >= this->valid_until;
}

template <size_t SECTOR_SIDE_LENGTH>
void CostField<SECTOR_SIDE_LENGTH>::clear_dirty() {
	this->valid_until = time::TIME_MAX;
}

template <size_t SECTOR_SIDE_LENGTH>
const curve::Array<cost_t, SECTOR_SIDE_LENGTH> &CostField<SECTOR_SIDE_LENGTH>::get_cost_history() const {
	return this->cell_cost_history;
};

} // namespace path
} // namespace openage
