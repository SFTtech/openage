// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <memory>


namespace openage::path {
class CostField;
class FlowField;

/**
 * Integrator for the flow field pathfinding algorithm.
 */
class Integrator {
public:
	Integrator() = default;
	~Integrator() = default;

	/**
     * Set the cost field.
     *
     * @param cost_field Cost field.
     */
	void set_cost_field(const std::shared_ptr<CostField> &cost_field);

	/**
     * Build the flow field for a target cell.
     *
     * @param target_x X coordinate of the target cell.
     * @param target_y Y coordinate of the target cell.
     *
     * @return Flow field.
     */
	std::shared_ptr<FlowField> build(size_t target_x, size_t target_y);

private:
	/**
     * Cost field.
     */
	std::shared_ptr<CostField> cost_field;
};

} // namespace openage::path
