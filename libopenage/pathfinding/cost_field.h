// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <vector>

#include "pathfinding/types.h"


namespace openage {
namespace coord {
struct tile;
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
	 * @param pos Coordinates of the cell.
	 * @return Cost at the specified position.
	 */
	cost_t get_cost(const coord::tile &pos) const;

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
	 * @param pos Coordinates of the cell.
	 * @param cost Cost to set.
	 */
	void set_cost(const coord::tile &pos, cost_t cost);

	/**
	 * Set the cost at a specified position.
	 *
	 * @param idx Index of the cell.
	 * @param cost Cost to set.
	 */
	void set_cost(size_t idx, cost_t cost);

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
	 */
	void set_costs(std::vector<cost_t> &&cells);

private:
	/**
	 * Side length of the field.
	 */
	size_t size;

	/**
	 * Cost field values.
	 */
	std::vector<cost_t> cells;
};

} // namespace path
} // namespace openage
