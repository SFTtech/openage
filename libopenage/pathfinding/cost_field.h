// Copyright 2024-2024 the openage authors. See copying.md for legal info.

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
	 * @param changed Time cost is set.
	 */
	void set_cost(const coord::tile_delta &pos, cost_t cost, time::time_t &changed);

	/**
	 * Set the cost at a specified position.
	 *
	 * @param x X-coordinate of the cell.
	 * @param y Y-coordinate of the cell.
	 * @param cost Cost to set.
	 * @param changed Time cost is set.
	 */
	void set_cost(size_t x, size_t y, cost_t cost, time::time_t &changed);

	/**
	 * Set the cost at a specified position.
	 *
	 * @param idx Index of the cell.
	 * @param cost Cost to set.
	 */
	void set_cost(size_t idx, cost_t cost);

	/**
	 * Set the cost at a specified position.
	 *
	 * @param idx Index of the cell.
	 * @param cost Cost to set.
	 * @param changed Time cost is set.
	 */
	void set_cost(size_t idx, cost_t cost, time::time_t &changed);

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
	 * @param changed Time cost is set.
	 */
	void set_costs(std::vector<cost_t> &&cells, time::time_t &changed);

	/**
	 * Check if the cost field is dirty at the specified time.
	 * 
	 * @param time Specified time to check.
	 */
	bool is_dirty(time::time_t &time);

private:
	/**
	 * Side length of the field.
	 */
	size_t size;

	time::time_t changed;

	/**
	 * Cost field values.
	 */
	std::vector<cost_t> cells;
};

} // namespace path
} // namespace openage
