// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <memory>
#include <unordered_set>
#include <vector>

#include "pathfinding/definitions.h"
#include "pathfinding/types.h"


namespace openage {
namespace coord {
struct tile_delta;
} // namespace coord

namespace path {
class IntegrationField;
class Portal;

class FlowField {
public:
	/**
	 * Create a square flow field with a specified size.
	 *
	 * @param size Side length of the field.
	 */
	FlowField(size_t size);

	/**
	 * Create a flow field from an existing integration field.
	 *
	 * @param integration_field Integration field.
	 */
	FlowField(const std::shared_ptr<IntegrationField> &integration_field);

	/**
	 * Get the size of the flow field.
	 *
	 * @return Size of the flow field.
	 */
	size_t get_size() const;

	/**
	 * Get the flow field value at a specified position.
	 *
	 * @param pos Coordinates of the cell (relative to field origin).
	 *
	 * @return Flowfield value at the specified position.
	 */
	flow_t get_cell(const coord::tile_delta &pos) const;

	/**
	 * Get the flow field value at a specified position.
	 *
	 * @param x X-coordinate of the cell.
	 * @param y Y-coordinate of the cell.
	 *
	 * @return Flowfield value at the specified position.
	 */
	flow_t get_cell(size_t x, size_t y) const;

	/**
	 * Get the flow field direction at a specified position.
	 *
	 * @param idx Index of the cell.
	 *
	 * @return Flowfield value at the specified position.
	 */
	flow_t get_cell(size_t idx) const;

	/**
	 * Get the flow field direction at a specified position.
	 *
	 * @param pos Coordinates of the cell (relative to field origin).
	 *
	 * @return Flowfield direction at the specified position.
	 */
	flow_dir_t get_dir(const coord::tile_delta &pos) const;

	/**
	 * Get the flow field direction at a specified position.
	 *
	 * @param x X-coordinate of the cell.
	 * @param y Y-coordinate of the cell.
	 *
	 * @return Flowfield direction at the specified position.
	 */
	flow_dir_t get_dir(size_t x, size_t y) const;

	/**
	 * Get the flow field direction at a specified position.
	 *
	 * @param idx Index of the cell.
	 *
	 * @return Flowfield direction at the specified position.
	 */
	flow_dir_t get_dir(size_t idx) const;

	/**
	 * Build the flow field.
	 *
	 * @param integration_field Integration field.
	 */
	void build(const std::shared_ptr<IntegrationField> &integration_field);

	/**
	 * Build the flow field for a portal.
	 *
	 * @param integration_field Integration field.
	 * @param other Integration field of the other sector.
	 * @param other_sector_id Sector ID of the other field.
	 * @param portal Portal connecting the two fields.
	 */
	void build(const std::shared_ptr<IntegrationField> &integration_field,
	           const std::shared_ptr<IntegrationField> &other,
	           sector_id_t other_sector_id,
	           const std::shared_ptr<Portal> &portal);

	/**
	 * Get the flow field values.
	 *
	 * @return Flow field values.
	 */
	const std::vector<flow_t> &get_cells() const;

	/**
	 * Reset the flow field values for rebuilding the field.
	 */
	void reset();

	/**
	 * Reset all flags that are dependent on the path target location.
	 *
	 * These flags should be removed when the field is cached and reused for
	 * other targets.
	 *
	 * Relevant flags are:
	 * - FLOW_LOS_MASK
	 * - FLOW_WAVEFRONT_BLOCKED_MASK
	 */
	void reset_dynamic_flags();

	/**
	 * Transfer dynamic flags from an integration field.
	 *
	 * These flags should be transferred when the field is copied from cache.
	 * Flow field directions are not altered.
	 *
	 * Relevant flags are:
	 * - FLOW_LOS_MASK
	 * - FLOW_WAVEFRONT_BLOCKED_MASK
	 *
	 * @param integration_field Integration field.
	 */
	void transfer_dynamic_flags(const std::shared_ptr<IntegrationField> &integration_field);

private:
	/**
	 * Side length of the field.
	 */
	size_t size;

	/**
	 * Flow field cells.
	 */
	std::vector<flow_t> cells;
};

} // namespace path
} // namespace openage
