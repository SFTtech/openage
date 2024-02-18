// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "pathfinding/types.h"


namespace openage::path {
class CostField;

/**
 * Integration field in the flow-field pathfinding algorithm.
 */
class IntegrationField {
public:
	/**
	 * Create a square integration field with a specified size.
	 *
	 * @param size Side length of the field.
	 */
	IntegrationField(size_t size);

	/**
	 * Get the size of the integration field.
	 *
	 * @return Size of the integration field.
	 */
	size_t get_size() const;

	/**
	 * Get the integration value at a specified position.
	 *
	 * @param x X coordinate.
	 * @param y Y coordinate.
	 * @return Integration value at the specified position.
	 */
	integrate_t get_cell(size_t x, size_t y) const;

	/**
	 * Get the integration value at a specified position.
	 *
	 * @param idx Index of the cell.
	 * @return Integration value at the specified position.
	 */
	integrate_t get_cell(size_t idx) const;

	/**
	 * Calculate the integration field for a target cell.
	 *
	 * @param cost_field Cost field to integrate.
	 * @param target_x X coordinate of the target cell.
	 * @param target_y Y coordinate of the target cell.
	 */
	void integrate(const std::shared_ptr<CostField> &cost_field,
	               size_t target_x,
	               size_t target_y);

	/**
	 * Get the integration field values.
	 *
	 * @return Integration field values.
	 */
	const std::vector<integrate_t> &get_cells() const;

private:
	/**
	 * Reset the integration field for a new integration.
	 */
	void reset();

	/**
	 * Update a cell in the integration field.
	 *
	 * @param idx Index of the cell that is updated.
	 * @param cell_cost Cost of the cell from the cost field.
	 * @param integrate_cost Integrated cost of the updating cell in the integration field.
	 *
	 * @return New integration value of the cell.
	 */
	integrate_t update_cell(size_t idx,
	                        cost_t cell_cost,
	                        integrate_t integrate_cost);

	/**
	 * Side length of the field.
	 */
	size_t size;

	/**
	 * Integration field values.
	 */
	std::vector<integrate_t> cells;
};

} // namespace openage::path
