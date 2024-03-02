// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "pathfinding/definitions.h"
#include "pathfinding/types.h"


namespace openage::path {
class IntegrationField;

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
	 * @param integrate_field Integration field.
	 */
	FlowField(const std::shared_ptr<IntegrationField> &integrate_field);

	/**
	 * Get the size of the flow field.
	 *
	 * @return Size of the flow field.
	 */
	size_t get_size() const;

	/**
	 * Get the flow field value at a specified position.
	 *
	 * @param x X coordinate.
	 * @param y Y coordinate.
	 *
	 * @return Flowfield value at the specified position.
	 */
	flow_t get_cell(size_t x, size_t y) const;

	/**
	 * Get the flow field direction at a specified position.
	 *
	 * @param x X coordinate.
	 * @param y Y coordinate.
	 *
	 * @return Flowfield direction at the specified position.
	 */
	flow_dir_t get_dir(size_t x, size_t y) const;

	/**
	 * Build the flow field.
	 *
	 * @param integrate_field Integration field.
	 */
	void build(const std::shared_ptr<IntegrationField> &integrate_field);

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

} // namespace openage::path
