// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <memory>


namespace openage {
namespace coord {
struct tile;
} // namespace coord

namespace path {
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
	 * Build the flow field for a target.
	 *
	 * @param cost_field Cost field.
	 * @param target Coordinates of the target cell.
	 *
	 * @return Flow field.
	 */
	std::shared_ptr<FlowField> build(const std::shared_ptr<CostField> &cost_field,
	                                 const coord::tile &target);

private:
	// TODO: Cache field results.
};

} // namespace path
} // namespace openage
