// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <utility>
#include <vector>

#include "pathfinding/cost_field.h"


namespace openage::path {

/**
 * Grid of cost fields for flow field pathfinding.
 */
class CostGrid {
public:
	/**
	 * Create a grid with a specified size and field size.
	 *
	 * @param width Width of the grid.
	 * @param height Height of the grid.
	 * @param field_size Size of the cost fields.
	 */
	CostGrid(size_t width,
	         size_t height,
	         size_t field_size);

	/**
	 * Get the size of the grid.
	 *
	 * @return Size of the grid (width x height).
	 */
	std::pair<size_t, size_t> get_size() const;

	/**
	 * Get the cost field at a specified position.
	 *
	 * @param x X coordinate.
	 * @param y Y coordinate.
	 * @return Cost field at the specified position.
	 */
	CostField &get_field(size_t x, size_t y);

private:
	/**
	 * Width of the grid.
	 */
	size_t width;

	/**
	 * Height of the grid.
	 */
	size_t height;

	/**
	 * Cost fields.
	 */
	std::vector<CostField> fields;
};


} // namespace openage::path
