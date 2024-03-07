// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <deque>
#include <memory>
#include <unordered_set>
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
	const integrate_t &get_cell(size_t x, size_t y) const;

	/**
	 * Get the integration value at a specified position.
	 *
	 * @param idx Index of the cell.
	 * @return Integration value at the specified position.
	 */
	const integrate_t &get_cell(size_t idx) const;

	/**
	 * Calculate the line-of-sight integration flags for a target cell.
	 *
	 * Returns a list of cells that are flagged as "wavefront blocked". These cells
	 * can be used as a starting point for the cost integration.
	 *
	 * @param cost_field Cost field to integrate.
	 * @param target_x X coordinate of the target cell.
	 * @param target_y Y coordinate of the target cell.
	 *
	 * @return Cells flagged as "wavefront blocked".
	 */
	std::vector<size_t> integrate_los(const std::shared_ptr<CostField> &cost_field,
	                                  size_t target_x,
	                                  size_t target_y);

	/**
	 * Calculate the cost integration field starting from a target cell.
	 *
	 * @param cost_field Cost field to integrate.
	 * @param target_x X coordinate of the target cell.
	 * @param target_y Y coordinate of the target cell.
	 * @param start_cells Cells flagged as "wavefront blocked" from a LOS pass.
	 */
	void integrate_cost(const std::shared_ptr<CostField> &cost_field,
	                    size_t target_x,
	                    size_t target_y);

	/**
	 * Calculate the cost integration field starting from a wavefront.
	 *
	 * @param cost_field Cost field to integrate.
	 * @param start_cells Cells flagged as "wavefront blocked" from a LOS pass.
	 */
	void integrate_cost(const std::shared_ptr<CostField> &cost_field,
	                    std::vector<size_t> &&start_cells = {});

	/**
	 * Get the integration field values.
	 *
	 * @return Integration field values.
	 */
	const std::vector<integrate_t> &get_cells() const;

	/**
	 * Reset the integration field for a new integration.
	 */
	void reset();

private:
	/**
	 * Update a neigbor cell during the cost integration process.
	 *
	 * @param idx Index of the neighbor cell that is updated.
	 * @param cell_cost Cost of the neighbor cell from the cost field.
	 * @param integrated_cost Current integrated cost of the updating cell in the integration field.
	 *
	 * @return New integration value of the cell.
	 */
	void update_neighbor(size_t idx,
	                     cost_t cell_cost,
	                     integrated_cost_t integrated_cost,
	                     std::deque<size_t> &open_list,
	                     std::unordered_set<size_t> &in_list);

	/**
	 * Get the LOS corners around a cell.
	 *
	 * @param cost_field Cost field to integrate.
	 * @param target_x X coordinate of the target.
	 * @param target_y Y coordinate of the target.
	 * @param blocker_x X coordinate of the cell blocking LOS.
	 * @param blocker_y Y coordinate of the cell blocking LOS.
	 */
	std::vector<std::pair<size_t, size_t>> get_los_corners(const std::shared_ptr<CostField> &cost_field,
	                                                       size_t target_x,
	                                                       size_t target_y,
	                                                       size_t blocker_x,
	                                                       size_t blocker_y);

	/**
	 * Get the cells in a bresenham's line between the corner cell and the field edge.
	 *
	 * @param target_x X coordinate of the target.
	 * @param target_y Y coordinate of the target.
	 * @param corner_x X coordinate edge of the LOS corner.
	 * @param corner_y Y coordinate edge of the LOS corner.
	 */
	std::vector<size_t> bresenhams_line(int target_x,
	                                    int target_y,
	                                    int corner_x,
	                                    int corner_y);

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
