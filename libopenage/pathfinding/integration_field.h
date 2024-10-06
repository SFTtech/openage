// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <deque>
#include <memory>
#include <unordered_set>
#include <vector>

#include "pathfinding/types.h"


namespace openage {
namespace coord {
struct tile_delta;
} // namespace coord

namespace path {
class CostField;
class Portal;

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
	 * @param pos Coordinates of the cell (relative to field origin).
	 * @return Integration value at the specified position.
	 */
	const integrated_t &get_cell(const coord::tile_delta &pos) const;

	/**
	 * Get the integration value at a specified position.
	 *
	 * @param x X-coordinate of the cell.
	 * @param y Y-coordinate of the cell.
	 * @return Integration value at the specified position.
	 */
	const integrated_t &get_cell(size_t x, size_t y) const;

	/**
	 * Get the integration value at a specified position.
	 *
	 * @param idx Index of the cell.
	 * @return Integration value at the specified position.
	 */
	const integrated_t &get_cell(size_t idx) const;

	/**
	 * Calculate the line-of-sight integration flags for a target cell.
	 *
	 * The target cell coordinates must lie within the field.
	 *
	 * Returns a list of cells that are flagged as "wavefront blocked". These cells
	 * can be used as a starting point for the cost integration.
	 *
	 * @param cost_field Cost field to integrate.
	 * @param target Coordinates of the target cell (relative to field origin).
	 *
	 * @return Cells flagged as "wavefront blocked".
	 */
	std::vector<size_t> integrate_los(const std::shared_ptr<CostField> &cost_field,
	                                  const coord::tile_delta &target);

	/**
	 * Calculate the line-of-sight integration flags starting from a portal to another
	 * integration field.
	 *
	 * Returns a list of cells that are flagged as "wavefront blocked". These cells
	 * can be used as a starting point for the cost integration.
	 *
	 * @param cost_field Cost field to integrate.
	 * @param other Integration field of the other sector.
	 * @param other_sector_id Sector ID of the other integration field.
	 * @param portal Portal connecting the two fields.
	 * @param target Coordinates of the target cell (relative to field origin).
	 *
	 * @return Cells flagged as "wavefront blocked".
	 */
	std::vector<size_t> integrate_los(const std::shared_ptr<CostField> &cost_field,
	                                  const std::shared_ptr<IntegrationField> &other,
	                                  sector_id_t other_sector_id,
	                                  const std::shared_ptr<Portal> &portal,
	                                  const coord::tile_delta &target);

	/**
	 * Calculate the line-of-sight integration flags for a target cell.
	 *
	 * Returns a list of cells that are flagged as "wavefront blocked". These cells
	 * can be used as a starting point for the cost integration.
	 *
	 * @param cost_field Cost field to integrate.
	 * @param target Coordinates of the target cell (relative to field origin).
	 * @param start_cost Integration cost for the start wave.
	 * @param start_wave Cells used for the first LOS integration wave. The wavefront
	 * 					 expands outwards from these cells.
	 *
	 * @return Cells flagged as "wavefront blocked".
	 */
	std::vector<size_t> integrate_los(const std::shared_ptr<CostField> &cost_field,
	                                  const coord::tile_delta &target,
	                                  integrated_cost_t start_cost,
	                                  std::vector<size_t> &&start_wave);

	/**
	 * Calculate the cost integration field starting from a target cell.
	 *
	 * @param cost_field Cost field to integrate.
	 * @param target Coordinates of the target cell.
	 */
	void integrate_cost(const std::shared_ptr<CostField> &cost_field,
	                    const coord::tile_delta &target);

	/**
	 * Calculate the cost integration field starting from a portal to another
	 * integration field.
	 *
	 * @param cost_field Cost field to integrate.
	 * @param other_sector_id Sector ID of the other integration field.
	 * @param portal Portal connecting the two fields.
	 */
	void integrate_cost(const std::shared_ptr<CostField> &cost_field,
	                    sector_id_t other_sector_id,
	                    const std::shared_ptr<Portal> &portal);

	/**
	 * Calculate the cost integration field starting from a wavefront.
	 *
	 * @param cost_field Cost field to integrate.
	 * @param start_cells Cells flagged as "wavefront blocked" from a LOS pass.
	 */
	void integrate_cost(const std::shared_ptr<CostField> &cost_field,
	                    std::vector<size_t> &&start_cells);

	/**
	 * Get the integration field values.
	 *
	 * @return Integration field values.
	 */
	const std::vector<integrated_t> &get_cells() const;

	/**
	 * Reset the integration field for a new integration.
	 */
	void reset();

	/**
	 * Reset all flags that are dependent on the path target location. These
	 * flags should be removed when the field is cached and reused for
	 * other targets.
	 *
	 * Relevant flags are:
	 * - INTEGRATE_LOS_MASK
	 * - INTEGRATE_WAVEFRONT_BLOCKED_MASK
	 * - INTEGRATE_FOUND_MASK
	 */
	void reset_dynamic_flags();

private:
	/**
	 * Update a neigbor cell during the cost integration process.
	 *
	 * @param idx Index of the neighbor cell that is updated.
	 * @param cell_cost Cost of the neighbor cell from the cost field.
	 * @param integrated_cost Current integrated cost of the updating cell in the integration field.
	 * @param wave List of cells that are part of the next wavefront.
	 *
	 * @return New integration value of the cell.
	 */
	void update_neighbor(size_t idx,
	                     cost_t cell_cost,
	                     integrated_cost_t integrated_cost,
	                     std::vector<size_t> &wave);

	/**
	 * Get the LOS corners around a cell.
	 *
	 * @param cost_field Cost field to integrate.
	 * @param target Cell coordinates of the target (relative to field origin).
	 * @param blocker Cell coordinates of the cell blocking LOS (relative to field origin).
	 *
	 * @return Field coordinates of the LOS corners.
	 */
	std::vector<std::pair<int, int>> get_los_corners(const std::shared_ptr<CostField> &cost_field,
	                                                 const coord::tile_delta &target,
	                                                 const coord::tile_delta &blocker);

	/**
	 * Get the cells in a bresenham's line between the corner cell and the field edge.
	 *
	 * This function is a modified version of the bresenham's line algorithm that
	 * retrieves the cells between the corner point and the field's edge, rather than
	 * the cells between two arbitrary points. We do this because the intersection
	 * point with the field edge is unknown.
	 *
	 * @param target Cell coordinates of the target (relative to field origin).
	 * @param corner_x X field coordinate edge of the LOS corner.
	 * @param corner_y Y field coordinate edge of the LOS corner.
	 *
	 * @return Cell indices of the LOS line.
	 */
	std::vector<size_t> bresenhams_line(const coord::tile_delta &target,
	                                    int corner_x,
	                                    int corner_y);

	/**
	 * Side length of the field.
	 */
	size_t size;

	/**
	 * Integration field values.
	 */
	std::vector<integrated_t> cells;
};

} // namespace path
} // namespace openage
