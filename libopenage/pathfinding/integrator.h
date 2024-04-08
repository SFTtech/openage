// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <memory>

#include "pathfinding/types.h"


namespace openage {
namespace coord {
struct tile;
} // namespace coord

namespace path {
class CostField;
class FlowField;
class IntegrationField;
class Portal;

/**
 * Integrator for the flow field pathfinding algorithm.
 */
class Integrator {
public:
	Integrator() = default;
	~Integrator() = default;

	/**
	 * Integrate the cost field for a target.
	 *
	 * @param cost_field Cost field.
	 * @param target Coordinates of the target cell.
	 *
	 * @return Integration field.
	 */
	std::shared_ptr<IntegrationField> integrate(const std::shared_ptr<CostField> &cost_field,
	                                            const coord::tile &target);

	/**
	 * Integrate the cost field from a portal.
	 *
	 * @param cost_field Cost field.
	 * @param other_sector_id Sector ID of the other side of the portal.
	 * @param portal Portal.
	 *
	 * @return Integration field.
	 */
	std::shared_ptr<IntegrationField> integrate(const std::shared_ptr<CostField> &cost_field,
	                                            sector_id_t other_sector_id,
	                                            const std::shared_ptr<Portal> &portal);

	/**
	 * Build the flow field from an integration field.
	 *
	 * @param integration_field Integration field.
	 *
	 * @return Flow field.
	 */
	std::shared_ptr<FlowField> build(const std::shared_ptr<IntegrationField> &integration_field);

	using build_return_t = std::pair<std::shared_ptr<IntegrationField>, std::shared_ptr<FlowField>>;

	/**
	 * Build the flow field for a target.
	 *
	 * @param cost_field Cost field.
	 * @param target Coordinates of the target cell.
	 *
	 * @return Flow field.
	 */
	build_return_t build(const std::shared_ptr<CostField> &cost_field,
	                     const coord::tile &target);
};

} // namespace path
} // namespace openage
