// Copyright 2024-2025 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <vector>

#include "pathfinding/types.h"
#include "time/time.h"


namespace openage {
namespace coord {
struct tile_delta;
} // namespace coord

namespace path {

/**
 * Cost field in the flow-field pathfinding algorithm.
 */
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
};

} // namespace path
} // namespace openage
